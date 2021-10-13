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
#include "hppe_qm_reg.h"
#include "hppe_qm.h"

sw_error_t
hppe_queue_tx_counter_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union queue_tx_counter_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + QUEUE_TX_COUNTER_TBL_ADDRESS + \
				index * QUEUE_TX_COUNTER_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_queue_tx_counter_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union queue_tx_counter_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + QUEUE_TX_COUNTER_TBL_ADDRESS + \
				index * QUEUE_TX_COUNTER_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_flush_cfg_get(
		a_uint32_t dev_id,
		union flush_cfg_u *value)
{
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + FLUSH_CFG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_flush_cfg_set(
		a_uint32_t dev_id,
		union flush_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + FLUSH_CFG_ADDRESS,
				value->val);
}

sw_error_t
hppe_in_mirror_priority_ctrl_get(
		a_uint32_t dev_id,
		union in_mirror_priority_ctrl_u *value)
{
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + IN_MIRROR_PRIORITY_CTRL_ADDRESS,
				&value->val);
}

sw_error_t
hppe_in_mirror_priority_ctrl_set(
		a_uint32_t dev_id,
		union in_mirror_priority_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + IN_MIRROR_PRIORITY_CTRL_ADDRESS,
				value->val);
}

sw_error_t
hppe_eg_mirror_priority_ctrl_get(
		a_uint32_t dev_id,
		union eg_mirror_priority_ctrl_u *value)
{
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + EG_MIRROR_PRIORITY_CTRL_ADDRESS,
				&value->val);
}

sw_error_t
hppe_eg_mirror_priority_ctrl_set(
		a_uint32_t dev_id,
		union eg_mirror_priority_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + EG_MIRROR_PRIORITY_CTRL_ADDRESS,
				value->val);
}

#ifndef IN_QM_MINI
sw_error_t
hppe_ucast_default_hash_get(
		a_uint32_t dev_id,
		union ucast_default_hash_u *value)
{
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UCAST_DEFAULT_HASH_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ucast_default_hash_set(
		a_uint32_t dev_id,
		union ucast_default_hash_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UCAST_DEFAULT_HASH_ADDRESS,
				value->val);
}

sw_error_t
hppe_spare_reg0_get(
		a_uint32_t dev_id,
		union spare_reg0_u *value)
{
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + SPARE_REG0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_spare_reg0_set(
		a_uint32_t dev_id,
		union spare_reg0_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + SPARE_REG0_ADDRESS,
				value->val);
}

sw_error_t
hppe_spare_reg1_get(
		a_uint32_t dev_id,
		union spare_reg1_u *value)
{
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + SPARE_REG1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_spare_reg1_set(
		a_uint32_t dev_id,
		union spare_reg1_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + SPARE_REG1_ADDRESS,
				value->val);
}

sw_error_t
hppe_qm_dbg_addr_get(
		a_uint32_t dev_id,
		union qm_dbg_addr_u *value)
{
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + QM_DBG_ADDR_ADDRESS,
				&value->val);
}

sw_error_t
hppe_qm_dbg_addr_set(
		a_uint32_t dev_id,
		union qm_dbg_addr_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + QM_DBG_ADDR_ADDRESS,
				value->val);
}

sw_error_t
hppe_qm_dbg_data_get(
		a_uint32_t dev_id,
		union qm_dbg_data_u *value)
{
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + QM_DBG_DATA_ADDRESS,
				&value->val);
}

sw_error_t
hppe_qm_dbg_data_set(
		a_uint32_t dev_id,
		union qm_dbg_data_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mcast_priority_map0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map0_u *value)
{
	if (index >= MCAST_PRIORITY_MAP0_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP0_ADDRESS + \
				index * MCAST_PRIORITY_MAP0_INC,
				&value->val);
}

sw_error_t
hppe_mcast_priority_map0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map0_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP0_ADDRESS + \
				index * MCAST_PRIORITY_MAP0_INC,
				value->val);
}

sw_error_t
hppe_mcast_priority_map1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map1_u *value)
{
	if (index >= MCAST_PRIORITY_MAP1_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP1_ADDRESS + \
				index * MCAST_PRIORITY_MAP1_INC,
				&value->val);
}

sw_error_t
hppe_mcast_priority_map1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map1_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP1_ADDRESS + \
				index * MCAST_PRIORITY_MAP1_INC,
				value->val);
}

sw_error_t
hppe_mcast_priority_map2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map2_u *value)
{
	if (index >= MCAST_PRIORITY_MAP2_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP2_ADDRESS + \
				index * MCAST_PRIORITY_MAP2_INC,
				&value->val);
}

sw_error_t
hppe_mcast_priority_map2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map2_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP2_ADDRESS + \
				index * MCAST_PRIORITY_MAP2_INC,
				value->val);
}

sw_error_t
hppe_mcast_priority_map3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map3_u *value)
{
	if (index >= MCAST_PRIORITY_MAP3_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP3_ADDRESS + \
				index * MCAST_PRIORITY_MAP3_INC,
				&value->val);
}

sw_error_t
hppe_mcast_priority_map3_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map3_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP3_ADDRESS + \
				index * MCAST_PRIORITY_MAP3_INC,
				value->val);
}

sw_error_t
hppe_mcast_priority_map4_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map4_u *value)
{
	if (index >= MCAST_PRIORITY_MAP4_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP4_ADDRESS + \
				index * MCAST_PRIORITY_MAP4_INC,
				&value->val);
}

sw_error_t
hppe_mcast_priority_map4_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map4_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP4_ADDRESS + \
				index * MCAST_PRIORITY_MAP4_INC,
				value->val);
}

sw_error_t
hppe_mcast_priority_map5_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map5_u *value)
{
	if (index >= MCAST_PRIORITY_MAP5_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP5_ADDRESS + \
				index * MCAST_PRIORITY_MAP5_INC,
				&value->val);
}

sw_error_t
hppe_mcast_priority_map5_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map5_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP5_ADDRESS + \
				index * MCAST_PRIORITY_MAP5_INC,
				value->val);
}

sw_error_t
hppe_mcast_priority_map6_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map6_u *value)
{
	if (index >= MCAST_PRIORITY_MAP6_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP6_ADDRESS + \
				index * MCAST_PRIORITY_MAP6_INC,
				&value->val);
}

sw_error_t
hppe_mcast_priority_map6_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map6_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP6_ADDRESS + \
				index * MCAST_PRIORITY_MAP6_INC,
				value->val);
}

sw_error_t
hppe_mcast_priority_map7_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map7_u *value)
{
	if (index >= MCAST_PRIORITY_MAP7_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP7_ADDRESS + \
				index * MCAST_PRIORITY_MAP7_INC,
				&value->val);
}

sw_error_t
hppe_mcast_priority_map7_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map7_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_PRIORITY_MAP7_ADDRESS + \
				index * MCAST_PRIORITY_MAP7_INC,
				value->val);
}

sw_error_t
hppe_agg_profile_cnt_en_get(
		a_uint32_t dev_id,
		union agg_profile_cnt_en_u *value)
{
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AGG_PROFILE_CNT_EN_ADDRESS,
				&value->val);
}

sw_error_t
hppe_agg_profile_cnt_en_set(
		a_uint32_t dev_id,
		union agg_profile_cnt_en_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AGG_PROFILE_CNT_EN_ADDRESS,
				value->val);
}

sw_error_t
hppe_uq_agg_profile_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_profile_cfg_u *value)
{
	if (index >= UQ_AGG_PROFILE_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UQ_AGG_PROFILE_CFG_ADDRESS + \
				index * UQ_AGG_PROFILE_CFG_INC,
				&value->val);
}

sw_error_t
hppe_uq_agg_profile_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_profile_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UQ_AGG_PROFILE_CFG_ADDRESS + \
				index * UQ_AGG_PROFILE_CFG_INC,
				value->val);
}

sw_error_t
hppe_mq_agg_profile_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_profile_cfg_u *value)
{
	if (index >= MQ_AGG_PROFILE_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MQ_AGG_PROFILE_CFG_ADDRESS + \
				index * MQ_AGG_PROFILE_CFG_INC,
				&value->val);
}

sw_error_t
hppe_mq_agg_profile_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_profile_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MQ_AGG_PROFILE_CFG_ADDRESS + \
				index * MQ_AGG_PROFILE_CFG_INC,
				value->val);
}

sw_error_t
hppe_grp_agg_profile_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_profile_cfg_u *value)
{
	if (index >= GRP_AGG_PROFILE_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + GRP_AGG_PROFILE_CFG_ADDRESS + \
				index * GRP_AGG_PROFILE_CFG_INC,
				&value->val);
}

sw_error_t
hppe_grp_agg_profile_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_profile_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + GRP_AGG_PROFILE_CFG_ADDRESS + \
				index * GRP_AGG_PROFILE_CFG_INC,
				value->val);
}

sw_error_t
hppe_uq_agg_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_in_profile_cnt_u *value)
{
	if (index >= UQ_AGG_IN_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UQ_AGG_IN_PROFILE_CNT_ADDRESS + \
				index * UQ_AGG_IN_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_uq_agg_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_in_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UQ_AGG_IN_PROFILE_CNT_ADDRESS + \
				index * UQ_AGG_IN_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_uq_agg_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_out_profile_cnt_u *value)
{
	if (index >= UQ_AGG_OUT_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UQ_AGG_OUT_PROFILE_CNT_ADDRESS + \
				index * UQ_AGG_OUT_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_uq_agg_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_out_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UQ_AGG_OUT_PROFILE_CNT_ADDRESS + \
				index * UQ_AGG_OUT_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_mq_agg_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_in_profile_cnt_u *value)
{
	if (index >= MQ_AGG_IN_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MQ_AGG_IN_PROFILE_CNT_ADDRESS + \
				index * MQ_AGG_IN_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_mq_agg_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_in_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MQ_AGG_IN_PROFILE_CNT_ADDRESS + \
				index * MQ_AGG_IN_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_mq_agg_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_out_profile_cnt_u *value)
{
	if (index >= MQ_AGG_OUT_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MQ_AGG_OUT_PROFILE_CNT_ADDRESS + \
				index * MQ_AGG_OUT_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_mq_agg_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_out_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MQ_AGG_OUT_PROFILE_CNT_ADDRESS + \
				index * MQ_AGG_OUT_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_grp_agg_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_in_profile_cnt_u *value)
{
	if (index >= GRP_AGG_IN_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + GRP_AGG_IN_PROFILE_CNT_ADDRESS + \
				index * GRP_AGG_IN_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_grp_agg_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_in_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + GRP_AGG_IN_PROFILE_CNT_ADDRESS + \
				index * GRP_AGG_IN_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_grp_agg_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_out_profile_cnt_u *value)
{
	if (index >= GRP_AGG_OUT_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + GRP_AGG_OUT_PROFILE_CNT_ADDRESS + \
				index * GRP_AGG_OUT_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_grp_agg_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_out_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + GRP_AGG_OUT_PROFILE_CNT_ADDRESS + \
				index * GRP_AGG_OUT_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_ucast_queue_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_queue_map_tbl_u *value)
{
	if (index >= UCAST_QUEUE_MAP_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UCAST_QUEUE_MAP_TBL_ADDRESS + \
				index * UCAST_QUEUE_MAP_TBL_INC,
				&value->val);
}
#endif

sw_error_t
hppe_ucast_queue_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_queue_map_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UCAST_QUEUE_MAP_TBL_ADDRESS + \
				index * UCAST_QUEUE_MAP_TBL_INC,
				value->val);
}

#ifndef IN_QM_MINI
sw_error_t
hppe_ucast_hash_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_hash_map_tbl_u *value)
{
	if (index >= UCAST_HASH_MAP_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UCAST_HASH_MAP_TBL_ADDRESS + \
				index * UCAST_HASH_MAP_TBL_INC,
				&value->val);
}

sw_error_t
hppe_ucast_hash_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_hash_map_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UCAST_HASH_MAP_TBL_ADDRESS + \
				index * UCAST_HASH_MAP_TBL_INC,
				value->val);
}

sw_error_t
hppe_ucast_priority_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_priority_map_tbl_u *value)
{
	if (index >= UCAST_PRIORITY_MAP_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UCAST_PRIORITY_MAP_TBL_ADDRESS + \
				index * UCAST_PRIORITY_MAP_TBL_INC,
				&value->val);
}

sw_error_t
hppe_ucast_priority_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_priority_map_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UCAST_PRIORITY_MAP_TBL_ADDRESS + \
				index * UCAST_PRIORITY_MAP_TBL_INC,
				value->val);
}

sw_error_t
hppe_mcast_queue_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_queue_map_tbl_u *value)
{
	if (index >= MCAST_QUEUE_MAP_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_QUEUE_MAP_TBL_ADDRESS + \
				index * MCAST_QUEUE_MAP_TBL_INC,
				&value->val);
}

sw_error_t
hppe_mcast_queue_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_queue_map_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MCAST_QUEUE_MAP_TBL_ADDRESS + \
				index * MCAST_QUEUE_MAP_TBL_INC,
				value->val);
}

sw_error_t
hppe_ac_mseq_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mseq_tbl_u *value)
{
	if (index >= AC_MSEQ_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_MSEQ_TBL_ADDRESS + \
				index * AC_MSEQ_TBL_INC,
				&value->val);
}

sw_error_t
hppe_ac_mseq_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mseq_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_MSEQ_TBL_ADDRESS + \
				index * AC_MSEQ_TBL_INC,
				value->val);
}
#endif

sw_error_t
hppe_ac_uni_queue_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_cfg_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_UNI_QUEUE_CFG_TBL_ADDRESS + \
				index * AC_UNI_QUEUE_CFG_TBL_INC,
				value->val,
				4);
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_cfg_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_UNI_QUEUE_CFG_TBL_ADDRESS + \
				index * AC_UNI_QUEUE_CFG_TBL_INC,
				value->val,
				4);
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_cfg_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_MUL_QUEUE_CFG_TBL_ADDRESS + \
				index * AC_MUL_QUEUE_CFG_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_cfg_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_MUL_QUEUE_CFG_TBL_ADDRESS + \
				index * AC_MUL_QUEUE_CFG_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_ac_grp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_cfg_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_GRP_CFG_TBL_ADDRESS + \
				index * AC_GRP_CFG_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_ac_grp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_cfg_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_GRP_CFG_TBL_ADDRESS + \
				index * AC_GRP_CFG_TBL_INC,
				value->val,
				3);
}

#ifndef IN_QM_MINI
sw_error_t
hppe_ac_uni_queue_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_cnt_tbl_u *value)
{
	if (index >= AC_UNI_QUEUE_CNT_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_UNI_QUEUE_CNT_TBL_ADDRESS + \
				index * AC_UNI_QUEUE_CNT_TBL_INC,
				&value->val);
}

sw_error_t
hppe_ac_uni_queue_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_cnt_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_UNI_QUEUE_CNT_TBL_ADDRESS + \
				index * AC_UNI_QUEUE_CNT_TBL_INC,
				value->val);
}

sw_error_t
hppe_ac_mul_queue_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_cnt_tbl_u *value)
{
	if (index >= AC_MUL_QUEUE_CNT_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_MUL_QUEUE_CNT_TBL_ADDRESS + \
				index * AC_MUL_QUEUE_CNT_TBL_INC,
				&value->val);
}

sw_error_t
hppe_ac_mul_queue_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_cnt_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_MUL_QUEUE_CNT_TBL_ADDRESS + \
				index * AC_MUL_QUEUE_CNT_TBL_INC,
				value->val);
}

sw_error_t
hppe_ac_grp_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_cnt_tbl_u *value)
{
	if (index >= AC_GRP_CNT_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_GRP_CNT_TBL_ADDRESS + \
				index * AC_GRP_CNT_TBL_INC,
				&value->val);
}

sw_error_t
hppe_ac_grp_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_cnt_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_GRP_CNT_TBL_ADDRESS + \
				index * AC_GRP_CNT_TBL_INC,
				value->val);
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_drop_state_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_UNI_QUEUE_DROP_STATE_TBL_ADDRESS + \
				index * AC_UNI_QUEUE_DROP_STATE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_drop_state_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_UNI_QUEUE_DROP_STATE_TBL_ADDRESS + \
				index * AC_UNI_QUEUE_DROP_STATE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_drop_state_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_MUL_QUEUE_DROP_STATE_TBL_ADDRESS + \
				index * AC_MUL_QUEUE_DROP_STATE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_drop_state_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_MUL_QUEUE_DROP_STATE_TBL_ADDRESS + \
				index * AC_MUL_QUEUE_DROP_STATE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_ac_grp_drop_state_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_drop_state_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_GRP_DROP_STATE_TBL_ADDRESS + \
				index * AC_GRP_DROP_STATE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_ac_grp_drop_state_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_drop_state_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + AC_GRP_DROP_STATE_TBL_ADDRESS + \
				index * AC_GRP_DROP_STATE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_oq_enq_opr_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_enq_opr_tbl_u *value)
{
	if (index >= OQ_ENQ_OPR_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_ENQ_OPR_TBL_ADDRESS + \
				index * OQ_ENQ_OPR_TBL_INC,
				&value->val);
}
#endif

sw_error_t
hppe_oq_enq_opr_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_enq_opr_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_ENQ_OPR_TBL_ADDRESS + \
				index * OQ_ENQ_OPR_TBL_INC,
				value->val);
}

#ifndef IN_QM_MINI
sw_error_t
hppe_oq_deq_opr_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_deq_opr_tbl_u *value)
{
	if (index >= OQ_DEQ_OPR_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_DEQ_OPR_TBL_ADDRESS + \
				index * OQ_DEQ_OPR_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_deq_opr_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_deq_opr_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_DEQ_OPR_TBL_ADDRESS + \
				index * OQ_DEQ_OPR_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_head_uni_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_head_uni_tbl_u *value)
{
	if (index >= OQ_HEAD_UNI_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_HEAD_UNI_TBL_ADDRESS + \
				index * OQ_HEAD_UNI_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_head_uni_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_head_uni_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_HEAD_UNI_TBL_ADDRESS + \
				index * OQ_HEAD_UNI_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_head_mul_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_head_mul_tbl_u *value)
{
	if (index >= OQ_HEAD_MUL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_HEAD_MUL_TBL_ADDRESS + \
				index * OQ_HEAD_MUL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_head_mul_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_head_mul_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_HEAD_MUL_TBL_ADDRESS + \
				index * OQ_HEAD_MUL_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_ll_uni_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_uni_tbl_u *value)
{
	if (index >= OQ_LL_UNI_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_UNI_TBL_ADDRESS + \
				index * OQ_LL_UNI_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_ll_uni_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_uni_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_UNI_TBL_ADDRESS + \
				index * OQ_LL_UNI_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p0_tbl_u *value)
{
	if (index >= OQ_LL_MUL_P0_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P0_TBL_ADDRESS + \
				index * OQ_LL_MUL_P0_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p0_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P0_TBL_ADDRESS + \
				index * OQ_LL_MUL_P0_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p1_tbl_u *value)
{
	if (index >= OQ_LL_MUL_P1_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P1_TBL_ADDRESS + \
				index * OQ_LL_MUL_P1_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p1_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P1_TBL_ADDRESS + \
				index * OQ_LL_MUL_P1_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p2_tbl_u *value)
{
	if (index >= OQ_LL_MUL_P2_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P2_TBL_ADDRESS + \
				index * OQ_LL_MUL_P2_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p2_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P2_TBL_ADDRESS + \
				index * OQ_LL_MUL_P2_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p3_tbl_u *value)
{
	if (index >= OQ_LL_MUL_P3_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P3_TBL_ADDRESS + \
				index * OQ_LL_MUL_P3_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p3_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P3_TBL_ADDRESS + \
				index * OQ_LL_MUL_P3_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p4_tbl_u *value)
{
	if (index >= OQ_LL_MUL_P4_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P4_TBL_ADDRESS + \
				index * OQ_LL_MUL_P4_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p4_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P4_TBL_ADDRESS + \
				index * OQ_LL_MUL_P4_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p5_tbl_u *value)
{
	if (index >= OQ_LL_MUL_P5_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P5_TBL_ADDRESS + \
				index * OQ_LL_MUL_P5_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p5_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P5_TBL_ADDRESS + \
				index * OQ_LL_MUL_P5_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p6_tbl_u *value)
{
	if (index >= OQ_LL_MUL_P6_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P6_TBL_ADDRESS + \
				index * OQ_LL_MUL_P6_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p6_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P6_TBL_ADDRESS + \
				index * OQ_LL_MUL_P6_TBL_INC,
				value->val);
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p7_tbl_u *value)
{
	if (index >= OQ_LL_MUL_P7_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P7_TBL_ADDRESS + \
				index * OQ_LL_MUL_P7_TBL_INC,
				&value->val);
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p7_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + OQ_LL_MUL_P7_TBL_ADDRESS + \
				index * OQ_LL_MUL_P7_TBL_INC,
				value->val);
}

sw_error_t
hppe_pkt_desp_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pkt_desp_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + PKT_DESP_TBL_ADDRESS + \
				index * PKT_DESP_TBL_INC,
				value->val,
				13);
}

sw_error_t
hppe_pkt_desp_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pkt_desp_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + PKT_DESP_TBL_ADDRESS + \
				index * PKT_DESP_TBL_INC,
				value->val,
				13);
}

sw_error_t
hppe_uni_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uni_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UNI_DROP_CNT_TBL_ADDRESS + \
				index * UNI_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_uni_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uni_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UNI_DROP_CNT_TBL_ADDRESS + \
				index * UNI_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p0_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p0_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P0_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P0_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p0_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p0_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P0_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P0_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p1_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p1_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P1_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P1_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p1_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p1_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P1_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P1_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p2_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p2_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P2_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P2_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p2_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p2_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P2_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P2_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p3_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p3_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P3_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P3_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p3_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p3_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P3_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P3_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p4_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p4_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P4_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P4_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p4_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p4_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P4_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P4_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p5_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p5_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P5_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P5_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p5_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p5_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P5_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P5_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p6_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p6_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P6_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P6_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p6_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p6_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P6_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P6_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p7_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p7_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P7_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P7_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_mul_p7_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p7_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + MUL_P7_DROP_CNT_TBL_ADDRESS + \
				index * MUL_P7_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_uq_agg_profile_map_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_profile_map_u *value)
{
	if (index >= UQ_AGG_PROFILE_MAP_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UQ_AGG_PROFILE_MAP_ADDRESS + \
				index * UQ_AGG_PROFILE_MAP_INC,
				&value->val);
}

sw_error_t
hppe_uq_agg_profile_map_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_profile_map_u *value)
{
	return hppe_reg_set(
				dev_id,
				QUEUE_MANAGER_BASE_ADDR + UQ_AGG_PROFILE_MAP_ADDRESS + \
				index * UQ_AGG_PROFILE_MAP_INC,
				value->val);
}

sw_error_t
hppe_flush_cfg_flush_busy_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.flush_busy;
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_busy_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flush_busy = value;
	ret = hppe_flush_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_qid_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.flush_qid;
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_qid_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flush_qid = value;
	ret = hppe_flush_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_dst_port_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.flush_dst_port;
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_dst_port_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flush_dst_port = value;
	ret = hppe_flush_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_all_queues_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.flush_all_queues;
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_all_queues_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flush_all_queues = value;
	ret = hppe_flush_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_wt_time_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.flush_wt_time;
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_wt_time_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flush_wt_time = value;
	ret = hppe_flush_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.flush_status;
	return ret;
}

sw_error_t
hppe_flush_cfg_flush_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union flush_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flush_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flush_status = value;
	ret = hppe_flush_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_in_mirror_priority_ctrl_priority_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union in_mirror_priority_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_mirror_priority_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.priority;
	return ret;
}

sw_error_t
hppe_in_mirror_priority_ctrl_priority_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union in_mirror_priority_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_mirror_priority_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.priority = value;
	ret = hppe_in_mirror_priority_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_mirror_priority_ctrl_priority_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union eg_mirror_priority_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_mirror_priority_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.priority;
	return ret;
}

sw_error_t
hppe_eg_mirror_priority_ctrl_priority_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union eg_mirror_priority_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_mirror_priority_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.priority = value;
	ret = hppe_eg_mirror_priority_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ucast_default_hash_hash_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union ucast_default_hash_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_default_hash_get(dev_id, &reg_val);
	*value = reg_val.bf.hash;
	return ret;
}

sw_error_t
hppe_ucast_default_hash_hash_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union ucast_default_hash_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_default_hash_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hash = value;
	ret = hppe_ucast_default_hash_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_spare_reg0_spare_reg0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union spare_reg0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_spare_reg0_get(dev_id, &reg_val);
	*value = reg_val.bf.spare_reg0;
	return ret;
}

sw_error_t
hppe_spare_reg0_spare_reg0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union spare_reg0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_spare_reg0_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.spare_reg0 = value;
	ret = hppe_spare_reg0_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_spare_reg1_spare_reg1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union spare_reg1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_spare_reg1_get(dev_id, &reg_val);
	*value = reg_val.bf.spare_reg1;
	return ret;
}

sw_error_t
hppe_spare_reg1_spare_reg1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union spare_reg1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_spare_reg1_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.spare_reg1 = value;
	ret = hppe_spare_reg1_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_qm_dbg_addr_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union qm_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_qm_dbg_addr_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_addr;
	return ret;
}

sw_error_t
hppe_qm_dbg_addr_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union qm_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_qm_dbg_addr_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dbg_addr = value;
	ret = hppe_qm_dbg_addr_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_qm_dbg_data_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union qm_dbg_data_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_qm_dbg_data_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_data;
	return ret;
}

sw_error_t
hppe_qm_dbg_data_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mcast_priority_map0_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mcast_priority_map0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_mcast_priority_map0_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mcast_priority_map0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_mcast_priority_map0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mcast_priority_map1_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mcast_priority_map1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_mcast_priority_map1_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mcast_priority_map1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_mcast_priority_map1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mcast_priority_map2_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mcast_priority_map2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_mcast_priority_map2_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mcast_priority_map2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_mcast_priority_map2_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mcast_priority_map3_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mcast_priority_map3_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map3_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_mcast_priority_map3_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mcast_priority_map3_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map3_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_mcast_priority_map3_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mcast_priority_map4_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mcast_priority_map4_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map4_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_mcast_priority_map4_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mcast_priority_map4_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map4_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_mcast_priority_map4_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mcast_priority_map5_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mcast_priority_map5_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map5_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_mcast_priority_map5_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mcast_priority_map5_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map5_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_mcast_priority_map5_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mcast_priority_map6_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mcast_priority_map6_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map6_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_mcast_priority_map6_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mcast_priority_map6_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map6_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_mcast_priority_map6_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mcast_priority_map7_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mcast_priority_map7_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map7_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_mcast_priority_map7_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mcast_priority_map7_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_priority_map7_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_mcast_priority_map7_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p2_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.mq_p2_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p2_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mq_p2_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.uq_en_1;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uq_en_1 = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p0_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.mq_p0_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p0_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mq_p0_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_grp_1_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.grp_1_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_grp_1_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_1_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_grp_0_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.grp_0_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_grp_0_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_0_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p6_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.mq_p6_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p6_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mq_p6_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_3_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.uq_en_3;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_3_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uq_en_3 = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p4_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.mq_p4_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p4_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mq_p4_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_2_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.uq_en_2;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_2_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uq_en_2 = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_5_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.uq_en_5;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_5_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uq_en_5 = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_6_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.uq_en_6;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_6_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uq_en_6 = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_grp_3_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.grp_3_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_grp_3_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_3_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_grp_2_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.grp_2_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_grp_2_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_2_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_4_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.uq_en_4;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_4_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uq_en_4 = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p7_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.mq_p7_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p7_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mq_p7_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_7_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.uq_en_7;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_7_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uq_en_7 = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_global_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.global_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_global_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.global_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p5_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.mq_p5_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p5_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mq_p5_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p1_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.mq_p1_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p1_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mq_p1_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.uq_en_0;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_uq_en_0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uq_en_0 = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p3_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.mq_p3_en;
	return ret;
}

sw_error_t
hppe_agg_profile_cnt_en_mq_p3_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union agg_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_agg_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mq_p3_en = value;
	ret = hppe_agg_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_uq_agg_profile_cfg_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union uq_agg_profile_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_profile_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.th_cfg;
	return ret;
}

sw_error_t
hppe_uq_agg_profile_cfg_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union uq_agg_profile_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_profile_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.th_cfg = value;
	ret = hppe_uq_agg_profile_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mq_agg_profile_cfg_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mq_agg_profile_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mq_agg_profile_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.th_cfg;
	return ret;
}

sw_error_t
hppe_mq_agg_profile_cfg_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mq_agg_profile_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mq_agg_profile_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.th_cfg = value;
	ret = hppe_mq_agg_profile_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_agg_profile_cfg_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union grp_agg_profile_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_agg_profile_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.th_cfg;
	return ret;
}

sw_error_t
hppe_grp_agg_profile_cfg_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union grp_agg_profile_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_agg_profile_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.th_cfg = value;
	ret = hppe_grp_agg_profile_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_uq_agg_in_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union uq_agg_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_in_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cnt;
	return ret;
}

sw_error_t
hppe_uq_agg_in_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union uq_agg_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_in_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cnt = value;
	ret = hppe_uq_agg_in_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_uq_agg_out_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union uq_agg_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_out_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cnt;
	return ret;
}

sw_error_t
hppe_uq_agg_out_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union uq_agg_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_out_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cnt = value;
	ret = hppe_uq_agg_out_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mq_agg_in_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mq_agg_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mq_agg_in_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cnt;
	return ret;
}

sw_error_t
hppe_mq_agg_in_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mq_agg_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mq_agg_in_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cnt = value;
	ret = hppe_mq_agg_in_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mq_agg_out_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mq_agg_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mq_agg_out_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cnt;
	return ret;
}

sw_error_t
hppe_mq_agg_out_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mq_agg_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mq_agg_out_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cnt = value;
	ret = hppe_mq_agg_out_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_agg_in_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union grp_agg_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_agg_in_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cnt;
	return ret;
}

sw_error_t
hppe_grp_agg_in_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union grp_agg_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_agg_in_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cnt = value;
	ret = hppe_grp_agg_in_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_agg_out_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union grp_agg_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_agg_out_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cnt;
	return ret;
}

sw_error_t
hppe_grp_agg_out_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union grp_agg_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_agg_out_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cnt = value;
	ret = hppe_grp_agg_out_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ucast_queue_map_tbl_profile_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ucast_queue_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_queue_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.profile_id;
	return ret;
}

sw_error_t
hppe_ucast_queue_map_tbl_profile_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ucast_queue_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_queue_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.profile_id = value;
	ret = hppe_ucast_queue_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ucast_queue_map_tbl_queue_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ucast_queue_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_queue_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.queue_id;
	return ret;
}

sw_error_t
hppe_ucast_queue_map_tbl_queue_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ucast_queue_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_queue_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.queue_id = value;
	ret = hppe_ucast_queue_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ucast_hash_map_tbl_hash_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ucast_hash_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_hash_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hash;
	return ret;
}

sw_error_t
hppe_ucast_hash_map_tbl_hash_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ucast_hash_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_hash_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hash = value;
	ret = hppe_ucast_hash_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ucast_priority_map_tbl_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ucast_priority_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_priority_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_ucast_priority_map_tbl_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ucast_priority_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ucast_priority_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_ucast_priority_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mcast_queue_map_tbl_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mcast_queue_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_queue_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.class;
	return ret;
}

sw_error_t
hppe_mcast_queue_map_tbl_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mcast_queue_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mcast_queue_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.class = value;
	ret = hppe_mcast_queue_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mseq_tbl_ac_mseq_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mseq_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mseq_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_mseq;
	return ret;
}

sw_error_t
hppe_ac_mseq_tbl_ac_mseq_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mseq_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mseq_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_mseq = value;
	ret = hppe_ac_mseq_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_yel_max_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_gap_grn_yel_max;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_yel_max_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_gap_grn_yel_max = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_wred_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_wred_en;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_wred_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_wred_en = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_ac_en;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_ac_en = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_red_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_red_resume_offset_1 << 9 | \
		reg_val.bf.ac_cfg_red_resume_offset_0;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_red_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_red_resume_offset_1 = value >> 9;
	reg_val.bf.ac_cfg_red_resume_offset_0 = value & (((a_uint64_t)1<<9)-1);
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_grp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_grp_id;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_grp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_grp_id = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_color_aware_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_color_aware;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_color_aware_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_color_aware = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_yel_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_yel_resume_offset;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_yel_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_yel_resume_offset = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_yel_min_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_gap_grn_yel_min_1 << 10 | \
		reg_val.bf.ac_cfg_gap_grn_yel_min_0;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_yel_min_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_gap_grn_yel_min_1 = value >> 10;
	reg_val.bf.ac_cfg_gap_grn_yel_min_0 = value & (((a_uint64_t)1<<10)-1);
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_weight_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_shared_weight;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_weight_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_shared_weight = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_dynamic_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_shared_dynamic;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_dynamic_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_shared_dynamic = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_red_max_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_gap_grn_red_max;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_red_max_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_gap_grn_red_max = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_pre_alloc_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_pre_alloc_limit;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_pre_alloc_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_pre_alloc_limit = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_force_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_force_ac_en;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_force_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_force_ac_en = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_red_min_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_gap_grn_red_min;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_red_min_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_gap_grn_red_min = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_grn_min_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_gap_grn_grn_min;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_grn_min_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_gap_grn_grn_min = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_ceiling_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_shared_ceiling;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_ceiling_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_shared_ceiling = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_grn_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_grn_resume_offset;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_grn_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_grn_resume_offset = value;
	ret = hppe_ac_uni_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_ac_en;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_ac_en = value;
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_red_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_red_resume_offset;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_red_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_red_resume_offset = value;
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_grp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_grp_id;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_grp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_grp_id = value;
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_color_aware_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_color_aware;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_color_aware_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_color_aware = value;
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_yel_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_yel_resume_offset_1 << 4 | \
		reg_val.bf.ac_cfg_yel_resume_offset_0;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_yel_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_yel_resume_offset_1 = value >> 4;
	reg_val.bf.ac_cfg_yel_resume_offset_0 = value & (((a_uint64_t)1<<4)-1);
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_pre_alloc_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_pre_alloc_limit;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_pre_alloc_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_pre_alloc_limit = value;
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_force_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_force_ac_en;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_force_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_force_ac_en = value;
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_shared_ceiling_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_shared_ceiling;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_shared_ceiling_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_shared_ceiling = value;
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_grn_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_grn_resume_offset;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_grn_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_grn_resume_offset = value;
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_gap_grn_yel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_gap_grn_yel_1 << 5 | \
		reg_val.bf.ac_cfg_gap_grn_yel_0;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_gap_grn_yel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_gap_grn_yel_1 = value >> 5;
	reg_val.bf.ac_cfg_gap_grn_yel_0 = value & (((a_uint64_t)1<<5)-1);
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_gap_grn_red_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_gap_grn_red;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_gap_grn_red_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_gap_grn_red = value;
	ret = hppe_ac_mul_queue_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_grn_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_grn_resume_offset;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_grn_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_grn_resume_offset = value;
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_dp_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_dp_thrd_1 << 7 | \
		reg_val.bf.ac_grp_dp_thrd_0;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_dp_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_dp_thrd_1 = value >> 7;
	reg_val.bf.ac_grp_dp_thrd_0 = value & (((a_uint64_t)1<<7)-1);
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_ac_en;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_ac_en = value;
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_palloc_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_palloc_limit;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_palloc_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_palloc_limit = value;
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_color_aware_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_color_aware;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_color_aware_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_color_aware = value;
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_red_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_red_resume_offset;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_red_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_red_resume_offset = value;
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_gap_grn_yel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_gap_grn_yel;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_gap_grn_yel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_gap_grn_yel = value;
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_force_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_cfg_force_ac_en;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_force_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_cfg_force_ac_en = value;
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_yel_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_yel_resume_offset_1 << 6 | \
		reg_val.bf.ac_grp_yel_resume_offset_0;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_yel_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_yel_resume_offset_1 = value >> 6;
	reg_val.bf.ac_grp_yel_resume_offset_0 = value & (((a_uint64_t)1<<6)-1);
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_gap_grn_red_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_gap_grn_red;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_gap_grn_red_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_gap_grn_red = value;
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_limit;
	return ret;
}

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_limit = value;
	ret = hppe_ac_grp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cnt_tbl_ac_uni_queue_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_uni_queue_cnt;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_cnt_tbl_ac_uni_queue_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_uni_queue_cnt = value;
	ret = hppe_ac_uni_queue_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cnt_tbl_ac_mul_queue_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_mul_queue_cnt;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_cnt_tbl_ac_mul_queue_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_mul_queue_cnt = value;
	ret = hppe_ac_mul_queue_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cnt_tbl_ac_grp_alloc_used_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_alloc_used;
	return ret;
}

sw_error_t
hppe_ac_grp_cnt_tbl_ac_grp_alloc_used_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_alloc_used = value;
	ret = hppe_ac_grp_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_cnt_tbl_ac_grp_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ac_grp_cnt;
	return ret;
}

sw_error_t
hppe_ac_grp_cnt_tbl_ac_grp_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ac_grp_cnt = value;
	ret = hppe_ac_grp_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_red_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.red_resume_thrd;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_red_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.red_resume_thrd = value;
	ret = hppe_ac_uni_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_red_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.red_drop_state;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_red_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.red_drop_state = value;
	ret = hppe_ac_uni_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_yel_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.yel_resume_thrd;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_yel_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.yel_resume_thrd = value;
	ret = hppe_ac_uni_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_grn_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.grn_drop_state;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_grn_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grn_drop_state = value;
	ret = hppe_ac_uni_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_yel_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.yel_drop_state;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_yel_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.yel_drop_state = value;
	ret = hppe_ac_uni_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_grn_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.grn_resume_thrd_1 << 10 | \
		reg_val.bf.grn_resume_thrd_0;
	return ret;
}

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_grn_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_uni_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_uni_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grn_resume_thrd_1 = value >> 10;
	reg_val.bf.grn_resume_thrd_0 = value & (((a_uint64_t)1<<10)-1);
	ret = hppe_ac_uni_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_red_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.red_resume_thrd;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_red_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.red_resume_thrd = value;
	ret = hppe_ac_mul_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_red_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.red_drop_state;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_red_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.red_drop_state = value;
	ret = hppe_ac_mul_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_yel_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.yel_resume_thrd;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_yel_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.yel_resume_thrd = value;
	ret = hppe_ac_mul_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_grn_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.grn_drop_state;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_grn_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grn_drop_state = value;
	ret = hppe_ac_mul_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_yel_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.yel_drop_state;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_yel_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.yel_drop_state = value;
	ret = hppe_ac_mul_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_grn_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.grn_resume_thrd_1 << 10 | \
		reg_val.bf.grn_resume_thrd_0;
	return ret;
}

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_grn_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_mul_queue_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_mul_queue_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grn_resume_thrd_1 = value >> 10;
	reg_val.bf.grn_resume_thrd_0 = value & (((a_uint64_t)1<<10)-1);
	ret = hppe_ac_mul_queue_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_red_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.red_resume_thrd;
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_red_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.red_resume_thrd = value;
	ret = hppe_ac_grp_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_red_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.red_drop_state;
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_red_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.red_drop_state = value;
	ret = hppe_ac_grp_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_yel_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.yel_resume_thrd;
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_yel_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.yel_resume_thrd = value;
	ret = hppe_ac_grp_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_grn_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.grn_drop_state;
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_grn_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grn_drop_state = value;
	ret = hppe_ac_grp_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_yel_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.yel_drop_state;
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_yel_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.yel_drop_state = value;
	ret = hppe_ac_grp_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_grn_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.grn_resume_thrd_1 << 10 | \
		reg_val.bf.grn_resume_thrd_0;
	return ret;
}

sw_error_t
hppe_ac_grp_drop_state_tbl_grn_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ac_grp_drop_state_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ac_grp_drop_state_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grn_resume_thrd_1 = value >> 10;
	reg_val.bf.grn_resume_thrd_0 = value & (((a_uint64_t)1<<10)-1);
	ret = hppe_ac_grp_drop_state_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_enq_opr_tbl_enq_disable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_enq_opr_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_enq_opr_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.enq_disable;
	return ret;
}

sw_error_t
hppe_oq_enq_opr_tbl_enq_disable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_enq_opr_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_enq_opr_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.enq_disable = value;
	ret = hppe_oq_enq_opr_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_deq_opr_tbl_deq_drop_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_deq_opr_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_deq_opr_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.deq_drop;
	return ret;
}

sw_error_t
hppe_oq_deq_opr_tbl_deq_drop_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_deq_opr_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_deq_opr_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.deq_drop = value;
	ret = hppe_oq_deq_opr_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_head_uni_tbl_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_head_uni_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_uni_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.head;
	return ret;
}

sw_error_t
hppe_oq_head_uni_tbl_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_head_uni_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_uni_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.head = value;
	ret = hppe_oq_head_uni_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_head_uni_tbl_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_head_uni_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_uni_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tail;
	return ret;
}

sw_error_t
hppe_oq_head_uni_tbl_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_head_uni_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_uni_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tail = value;
	ret = hppe_oq_head_uni_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_head_uni_tbl_empty_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_head_uni_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_uni_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.empty;
	return ret;
}

sw_error_t
hppe_oq_head_uni_tbl_empty_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_head_uni_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_uni_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.empty = value;
	ret = hppe_oq_head_uni_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.head;
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.head = value;
	ret = hppe_oq_head_mul_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tail;
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tail = value;
	ret = hppe_oq_head_mul_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ingress_mirr;
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ingress_mirr = value;
	ret = hppe_oq_head_mul_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_empty_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.empty;
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_empty_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.empty = value;
	ret = hppe_oq_head_mul_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.egress_mirr;
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.egress_mirr = value;
	ret = hppe_oq_head_mul_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.normal_fwd;
	return ret;
}

sw_error_t
hppe_oq_head_mul_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_head_mul_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_head_mul_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.normal_fwd = value;
	ret = hppe_oq_head_mul_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_uni_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_uni_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_uni_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_pointer;
	return ret;
}

sw_error_t
hppe_oq_ll_uni_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_uni_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_uni_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_pointer = value;
	ret = hppe_oq_ll_uni_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p0_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p0_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_pointer;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p0_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p0_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_pointer = value;
	ret = hppe_oq_ll_mul_p0_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p0_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p0_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ingress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p0_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p0_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ingress_mirr = value;
	ret = hppe_oq_ll_mul_p0_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p0_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p0_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.egress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p0_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p0_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.egress_mirr = value;
	ret = hppe_oq_ll_mul_p0_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p0_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p0_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.normal_fwd;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p0_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p0_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p0_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.normal_fwd = value;
	ret = hppe_oq_ll_mul_p0_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p1_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p1_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_pointer;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p1_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p1_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_pointer = value;
	ret = hppe_oq_ll_mul_p1_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p1_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p1_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ingress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p1_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p1_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ingress_mirr = value;
	ret = hppe_oq_ll_mul_p1_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p1_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p1_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.egress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p1_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p1_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.egress_mirr = value;
	ret = hppe_oq_ll_mul_p1_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p1_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p1_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.normal_fwd;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p1_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p1_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p1_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.normal_fwd = value;
	ret = hppe_oq_ll_mul_p1_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p2_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p2_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_pointer;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p2_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p2_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_pointer = value;
	ret = hppe_oq_ll_mul_p2_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p2_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p2_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ingress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p2_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p2_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ingress_mirr = value;
	ret = hppe_oq_ll_mul_p2_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p2_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p2_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.egress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p2_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p2_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.egress_mirr = value;
	ret = hppe_oq_ll_mul_p2_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p2_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p2_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.normal_fwd;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p2_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p2_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p2_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.normal_fwd = value;
	ret = hppe_oq_ll_mul_p2_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p3_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p3_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_pointer;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p3_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p3_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_pointer = value;
	ret = hppe_oq_ll_mul_p3_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p3_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p3_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ingress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p3_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p3_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ingress_mirr = value;
	ret = hppe_oq_ll_mul_p3_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p3_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p3_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.egress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p3_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p3_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.egress_mirr = value;
	ret = hppe_oq_ll_mul_p3_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p3_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p3_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.normal_fwd;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p3_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p3_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p3_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.normal_fwd = value;
	ret = hppe_oq_ll_mul_p3_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p4_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p4_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_pointer;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p4_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p4_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_pointer = value;
	ret = hppe_oq_ll_mul_p4_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p4_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p4_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ingress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p4_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p4_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ingress_mirr = value;
	ret = hppe_oq_ll_mul_p4_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p4_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p4_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.egress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p4_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p4_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.egress_mirr = value;
	ret = hppe_oq_ll_mul_p4_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p4_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p4_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.normal_fwd;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p4_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p4_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p4_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.normal_fwd = value;
	ret = hppe_oq_ll_mul_p4_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p5_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p5_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_pointer;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p5_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p5_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_pointer = value;
	ret = hppe_oq_ll_mul_p5_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p5_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p5_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ingress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p5_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p5_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ingress_mirr = value;
	ret = hppe_oq_ll_mul_p5_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p5_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p5_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.egress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p5_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p5_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.egress_mirr = value;
	ret = hppe_oq_ll_mul_p5_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p5_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p5_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.normal_fwd;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p5_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p5_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p5_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.normal_fwd = value;
	ret = hppe_oq_ll_mul_p5_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p6_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p6_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_pointer;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p6_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p6_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_pointer = value;
	ret = hppe_oq_ll_mul_p6_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p6_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p6_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ingress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p6_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p6_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ingress_mirr = value;
	ret = hppe_oq_ll_mul_p6_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p6_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p6_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.egress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p6_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p6_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.egress_mirr = value;
	ret = hppe_oq_ll_mul_p6_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p6_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p6_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.normal_fwd;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p6_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p6_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p6_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.normal_fwd = value;
	ret = hppe_oq_ll_mul_p6_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p7_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p7_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_pointer;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p7_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p7_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_pointer = value;
	ret = hppe_oq_ll_mul_p7_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p7_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p7_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ingress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p7_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p7_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ingress_mirr = value;
	ret = hppe_oq_ll_mul_p7_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p7_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p7_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.egress_mirr;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p7_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p7_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.egress_mirr = value;
	ret = hppe_oq_ll_mul_p7_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union oq_ll_mul_p7_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p7_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.normal_fwd;
	return ret;
}

sw_error_t
hppe_oq_ll_mul_p7_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union oq_ll_mul_p7_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_oq_ll_mul_p7_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.normal_fwd = value;
	ret = hppe_oq_ll_mul_p7_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ip_addr_index_valid;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ip_addr_index_valid = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_route_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.route_flag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_route_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.route_flag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_cpcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_cpcp;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_cpcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_cpcp = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_pkt_l3_edit_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.pkt_l3_edit_bypass;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_pkt_l3_edit_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.pkt_l3_edit_bypass = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_ctag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_ctag_fmt;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_ctag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_ctag_fmt = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_fake_mac_header_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.fake_mac_header;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_fake_mac_header_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.fake_mac_header = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_acl_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.acl_index;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_acl_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.acl_index = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_l4_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.l4_type;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_l4_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.l4_type = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_svid;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_svid = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_sdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_sdei;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_sdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_sdei = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_fc_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.fc_en;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_fc_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.fc_en = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_packet_length_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.packet_length_1 << 8 | \
		reg_val.bf1.packet_length_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_packet_length_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.packet_length_1 = value >> 8;
	reg_val.bf1.packet_length_0 = value & (((a_uint64_t)1<<8)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_rx_ts_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf1.rx_ts_1 << 24 | \
		reg_val.bf1.rx_ts_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_rx_ts_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.rx_ts_1 = value >> 24;
	reg_val.bf1.rx_ts_0 = value & (((a_uint64_t)1<<24)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ts_dir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ts_dir;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ts_dir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ts_dir = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_chg_port_vp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.chg_port_vp;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_chg_port_vp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.chg_port_vp = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_pri;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_pri = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_one_enq_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.one_enq_flag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_one_enq_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.one_enq_flag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_fc_grp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.fc_grp_id;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_fc_grp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.fc_grp_id = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_fake_l2_prot_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.fake_l2_prot;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_fake_l2_prot_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.fake_l2_prot = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_org_src_port_vp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.org_src_port_vp;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_org_src_port_vp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.org_src_port_vp = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_hash_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.hash_flag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_hash_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.hash_flag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_stag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_stag_fmt;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_stag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_stag_fmt = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_service_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.service_code_1 << 2 | \
		reg_val.bf1.service_code_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_service_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.service_code_1 = value >> 2;
	reg_val.bf1.service_code_0 = value & (((a_uint64_t)1<<2)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_rx_ptp_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.rx_ptp_type;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_rx_ptp_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.rx_ptp_type = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_mac_da_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf1.mac_da_1 << 18 | \
		reg_val.bf1.mac_da_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_mac_da_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.mac_da_1 = value >> 18;
	reg_val.bf1.mac_da_0 = value & (((a_uint64_t)1<<18)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_cpu_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.cpu_code;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_cpu_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.cpu_code = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_eg_vlan_tag_fmt_bypass_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.eg_vlan_tag_fmt_bypass_en;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_eg_vlan_tag_fmt_bypass_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.eg_vlan_tag_fmt_bypass_en = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ip_addr_index_type;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ip_addr_index_type = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_cvid_1 << 9 | \
		reg_val.bf1.int_cvid_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_cvid_1 = value >> 9;
	reg_val.bf1.int_cvid_0 = value & (((a_uint64_t)1<<9)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_eg_vlan_xlt_bypass_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.eg_vlan_xlt_bypass_en;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_eg_vlan_xlt_bypass_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.eg_vlan_xlt_bypass_en = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_hash_value_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.hash_value_1 << 5 | \
		reg_val.bf1.hash_value_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_hash_value_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.hash_value_1 = value >> 5;
	reg_val.bf1.hash_value_0 = value & (((a_uint64_t)1<<5)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_stag_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.stag_flag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_stag_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.stag_flag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_dst_l3_if_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.dst_l3_if;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_dst_l3_if_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.dst_l3_if = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_cdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_cdei;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_cdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_cdei = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_edma_vp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.edma_vp;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_edma_vp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.edma_vp = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ac_group_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ac_group_bitmap;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ac_group_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ac_group_bitmap = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_vp_tx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.vp_tx_cnt_en;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_vp_tx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.vp_tx_cnt_en = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_src_port_vp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.src_port_vp;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_src_port_vp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.src_port_vp = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_nat_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.nat_action;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_nat_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.nat_action = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_dscp_update_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.dscp_update;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_dscp_update_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.dscp_update = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_pppoe_strip_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.pppoe_strip_flag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_pppoe_strip_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.pppoe_strip_flag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_snap_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.snap_flag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_snap_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.snap_flag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_vsi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.vsi;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_vsi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.vsi = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_pkt_l2_edit_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.pkt_l2_edit_bypass;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_pkt_l2_edit_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.pkt_l2_edit_bypass = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_tx_ptp_tag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf0.tx_ptp_tag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_tx_ptp_tag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf0.tx_ptp_tag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ip_addr_index_1 << 1 | \
		reg_val.bf1.ip_addr_index_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ip_addr_index_1 = value >> 1;
	reg_val.bf1.ip_addr_index_0 = value & (((a_uint64_t)1<<1)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_dp;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_dp = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_src_pn_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.src_pn;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_src_pn_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.src_pn = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_tx_ts_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf0.tx_ts_en;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_tx_ts_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf0.tx_ts_en = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_l4_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.l4_offset_1 << 6 | \
		reg_val.bf1.l4_offset_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_l4_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.l4_offset_1 = value >> 6;
	reg_val.bf1.l4_offset_0 = value & (((a_uint64_t)1<<6)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ttl_update_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ttl_update;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ttl_update_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ttl_update = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_napt_port_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.napt_port;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_napt_port_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.napt_port = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_napt_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.napt_addr_1 << 13 | \
		reg_val.bf1.napt_addr_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_napt_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.napt_addr_1 = value >> 13;
	reg_val.bf1.napt_addr_0 = value & (((a_uint64_t)1<<13)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_copy_cpu_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.copy_cpu_flag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_copy_cpu_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.copy_cpu_flag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ttl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ttl_1 << 3 | \
		reg_val.bf1.ttl_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ttl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ttl_1 = value >> 3;
	reg_val.bf1.ttl_0 = value & (((a_uint64_t)1<<3)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_l3_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.l3_offset;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_l3_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.l3_offset = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_rsv0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.rsv0_1 << 3 | \
		reg_val.bf1.rsv0_0;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_rsv0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.rsv0_1 = value >> 3;
	reg_val.bf1.rsv0_0 = value & (((a_uint64_t)1<<3)-1);
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_next_header_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.next_header;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_next_header_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.next_header = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_acl_index_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.acl_index_valid;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_acl_index_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.acl_index_valid = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_rx_ts_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.rx_ts_valid;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_rx_ts_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.rx_ts_valid = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_dscp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.dscp;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_dscp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.dscp = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_acl_index_toggle_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.acl_index_toggle;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_acl_index_toggle_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.acl_index_toggle = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ctag_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ctag_flag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ctag_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ctag_flag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_toggle_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ip_addr_index_toggle;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_toggle_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ip_addr_index_toggle = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_tx_os_correction_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf0.tx_os_correction_en;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_tx_os_correction_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf0.tx_os_correction_en = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_spcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.int_spcp;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_int_spcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.int_spcp = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_pppoe_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.pppoe_flag;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_pppoe_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.pppoe_flag = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_l3_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.l3_type;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_l3_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.l3_type = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_vsi_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.vsi_valid;
	return ret;
}

sw_error_t
hppe_pkt_desp_tbl_vsi_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pkt_desp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pkt_desp_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.vsi_valid = value;
	ret = hppe_pkt_desp_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_uni_drop_cnt_tbl_uni_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union uni_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uni_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.uni_drop_byte_1 << 32 | \
		reg_val.bf.uni_drop_byte_0;
	return ret;
}

sw_error_t
hppe_uni_drop_cnt_tbl_uni_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union uni_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uni_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uni_drop_byte_1 = value >> 32;
	reg_val.bf.uni_drop_byte_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_uni_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_uni_drop_cnt_tbl_uni_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union uni_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uni_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.uni_drop_pkt;
	return ret;
}

sw_error_t
hppe_uni_drop_cnt_tbl_uni_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union uni_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uni_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uni_drop_pkt = value;
	ret = hppe_uni_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p0_drop_cnt_tbl_mul_p0_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mul_p0_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p0_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mul_p0_drop_pkt;
	return ret;
}

sw_error_t
hppe_mul_p0_drop_cnt_tbl_mul_p0_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mul_p0_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p0_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p0_drop_pkt = value;
	ret = hppe_mul_p0_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p0_drop_cnt_tbl_mul_p0_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union mul_p0_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p0_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mul_p0_drop_byte_1 << 32 | \
		reg_val.bf.mul_p0_drop_byte_0;
	return ret;
}

sw_error_t
hppe_mul_p0_drop_cnt_tbl_mul_p0_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union mul_p0_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p0_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p0_drop_byte_1 = value >> 32;
	reg_val.bf.mul_p0_drop_byte_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_mul_p0_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p1_drop_cnt_tbl_mul_p1_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union mul_p1_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p1_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mul_p1_drop_byte_1 << 32 | \
		reg_val.bf.mul_p1_drop_byte_0;
	return ret;
}

sw_error_t
hppe_mul_p1_drop_cnt_tbl_mul_p1_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union mul_p1_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p1_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p1_drop_byte_1 = value >> 32;
	reg_val.bf.mul_p1_drop_byte_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_mul_p1_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p1_drop_cnt_tbl_mul_p1_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mul_p1_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p1_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mul_p1_drop_pkt;
	return ret;
}

sw_error_t
hppe_mul_p1_drop_cnt_tbl_mul_p1_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mul_p1_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p1_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p1_drop_pkt = value;
	ret = hppe_mul_p1_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p2_drop_cnt_tbl_mul_p2_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mul_p2_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p2_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mul_p2_drop_pkt;
	return ret;
}

sw_error_t
hppe_mul_p2_drop_cnt_tbl_mul_p2_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mul_p2_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p2_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p2_drop_pkt = value;
	ret = hppe_mul_p2_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p2_drop_cnt_tbl_mul_p2_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union mul_p2_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p2_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mul_p2_drop_byte_1 << 32 | \
		reg_val.bf.mul_p2_drop_byte_0;
	return ret;
}

sw_error_t
hppe_mul_p2_drop_cnt_tbl_mul_p2_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union mul_p2_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p2_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p2_drop_byte_1 = value >> 32;
	reg_val.bf.mul_p2_drop_byte_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_mul_p2_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p3_drop_cnt_tbl_mul_p3_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mul_p3_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p3_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mul_p3_drop_pkt;
	return ret;
}

sw_error_t
hppe_mul_p3_drop_cnt_tbl_mul_p3_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mul_p3_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p3_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p3_drop_pkt = value;
	ret = hppe_mul_p3_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p3_drop_cnt_tbl_mul_p3_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union mul_p3_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p3_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mul_p3_drop_byte_1 << 32 | \
		reg_val.bf.mul_p3_drop_byte_0;
	return ret;
}

sw_error_t
hppe_mul_p3_drop_cnt_tbl_mul_p3_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union mul_p3_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p3_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p3_drop_byte_1 = value >> 32;
	reg_val.bf.mul_p3_drop_byte_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_mul_p3_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p4_drop_cnt_tbl_mul_p4_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mul_p4_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p4_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mul_p4_drop_pkt;
	return ret;
}

sw_error_t
hppe_mul_p4_drop_cnt_tbl_mul_p4_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mul_p4_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p4_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p4_drop_pkt = value;
	ret = hppe_mul_p4_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p4_drop_cnt_tbl_mul_p4_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union mul_p4_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p4_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mul_p4_drop_byte_1 << 32 | \
		reg_val.bf.mul_p4_drop_byte_0;
	return ret;
}

sw_error_t
hppe_mul_p4_drop_cnt_tbl_mul_p4_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union mul_p4_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p4_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p4_drop_byte_1 = value >> 32;
	reg_val.bf.mul_p4_drop_byte_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_mul_p4_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p5_drop_cnt_tbl_mul_p5_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union mul_p5_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p5_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mul_p5_drop_byte_1 << 32 | \
		reg_val.bf.mul_p5_drop_byte_0;
	return ret;
}

sw_error_t
hppe_mul_p5_drop_cnt_tbl_mul_p5_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union mul_p5_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p5_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p5_drop_byte_1 = value >> 32;
	reg_val.bf.mul_p5_drop_byte_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_mul_p5_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p5_drop_cnt_tbl_mul_p5_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mul_p5_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p5_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mul_p5_drop_pkt;
	return ret;
}

sw_error_t
hppe_mul_p5_drop_cnt_tbl_mul_p5_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mul_p5_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p5_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p5_drop_pkt = value;
	ret = hppe_mul_p5_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p6_drop_cnt_tbl_mul_p6_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union mul_p6_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p6_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mul_p6_drop_byte_1 << 32 | \
		reg_val.bf.mul_p6_drop_byte_0;
	return ret;
}

sw_error_t
hppe_mul_p6_drop_cnt_tbl_mul_p6_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union mul_p6_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p6_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p6_drop_byte_1 = value >> 32;
	reg_val.bf.mul_p6_drop_byte_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_mul_p6_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p6_drop_cnt_tbl_mul_p6_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mul_p6_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p6_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mul_p6_drop_pkt;
	return ret;
}

sw_error_t
hppe_mul_p6_drop_cnt_tbl_mul_p6_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mul_p6_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p6_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p6_drop_pkt = value;
	ret = hppe_mul_p6_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p7_drop_cnt_tbl_mul_p7_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mul_p7_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p7_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mul_p7_drop_pkt;
	return ret;
}

sw_error_t
hppe_mul_p7_drop_cnt_tbl_mul_p7_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mul_p7_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p7_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p7_drop_pkt = value;
	ret = hppe_mul_p7_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mul_p7_drop_cnt_tbl_mul_p7_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union mul_p7_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p7_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mul_p7_drop_byte_1 << 32 | \
		reg_val.bf.mul_p7_drop_byte_0;
	return ret;
}

sw_error_t
hppe_mul_p7_drop_cnt_tbl_mul_p7_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union mul_p7_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mul_p7_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_p7_drop_byte_1 = value >> 32;
	reg_val.bf.mul_p7_drop_byte_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_mul_p7_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_uq_agg_profile_map_qid_2_agg_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union uq_agg_profile_map_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_profile_map_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qid_2_agg_id;
	return ret;
}

sw_error_t
hppe_uq_agg_profile_map_qid_2_agg_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union uq_agg_profile_map_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_profile_map_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qid_2_agg_id = value;
	ret = hppe_uq_agg_profile_map_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_uq_agg_profile_map_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union uq_agg_profile_map_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_profile_map_get(dev_id, index, &reg_val);
	*value = reg_val.bf.enable;
	return ret;
}

sw_error_t
hppe_uq_agg_profile_map_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union uq_agg_profile_map_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_uq_agg_profile_map_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.enable = value;
	ret = hppe_uq_agg_profile_map_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_queue_tx_counter_tbl_tx_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union queue_tx_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_queue_tx_counter_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.tx_bytes_1 << 32 | \
		reg_val.bf.tx_bytes_0;
	return ret;
}

sw_error_t
hppe_queue_tx_counter_tbl_tx_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union queue_tx_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_queue_tx_counter_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_bytes_1 = value >> 32;
	reg_val.bf.tx_bytes_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_queue_tx_counter_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_queue_tx_counter_tbl_tx_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union queue_tx_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_queue_tx_counter_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_packets;
	return ret;
}

sw_error_t
hppe_queue_tx_counter_tbl_tx_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union queue_tx_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_queue_tx_counter_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_packets = value;
	ret = hppe_queue_tx_counter_tbl_set(dev_id, index, &reg_val);
	return ret;
}
#endif
