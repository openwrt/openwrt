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
#include "hppe_bm_reg.h"
#include "hppe_bm.h"

#ifndef IN_BM_MINI
sw_error_t
hppe_fb_fifo_cfg_get(
		a_uint32_t dev_id,
		union fb_fifo_cfg_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + FB_FIFO_CFG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_fb_fifo_cfg_set(
		a_uint32_t dev_id,
		union fb_fifo_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + FB_FIFO_CFG_ADDRESS,
				value->val);
}

sw_error_t
hppe_fp_fifo_cfg_get(
		a_uint32_t dev_id,
		union fp_fifo_cfg_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + FP_FIFO_CFG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_fp_fifo_cfg_set(
		a_uint32_t dev_id,
		union fp_fifo_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + FP_FIFO_CFG_ADDRESS,
				value->val);
}

sw_error_t
hppe_deq_fifo_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union deq_fifo_cfg_u *value)
{
	if (index >= DEQ_FIFO_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + DEQ_FIFO_CFG_ADDRESS + \
				index * DEQ_FIFO_CFG_INC,
				&value->val);
}

sw_error_t
hppe_deq_fifo_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union deq_fifo_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + DEQ_FIFO_CFG_ADDRESS + \
				index * DEQ_FIFO_CFG_INC,
				value->val);
}

sw_error_t
hppe_tick_dly_cfg_get(
		a_uint32_t dev_id,
		union tick_dly_cfg_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + TICK_DLY_CFG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_tick_dly_cfg_set(
		a_uint32_t dev_id,
		union tick_dly_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + TICK_DLY_CFG_ADDRESS,
				value->val);
}

sw_error_t
hppe_bm_rsv_0_get(
		a_uint32_t dev_id,
		union bm_rsv_0_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + BM_RSV_0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_bm_rsv_0_set(
		a_uint32_t dev_id,
		union bm_rsv_0_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + BM_RSV_0_ADDRESS,
				value->val);
}

sw_error_t
hppe_bm_rsv_1_get(
		a_uint32_t dev_id,
		union bm_rsv_1_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + BM_RSV_1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_bm_rsv_1_set(
		a_uint32_t dev_id,
		union bm_rsv_1_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + BM_RSV_1_ADDRESS,
				value->val);
}

sw_error_t
hppe_bm_dbg_addr_get(
		a_uint32_t dev_id,
		union bm_dbg_addr_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + BM_DBG_ADDR_ADDRESS,
				&value->val);
}

sw_error_t
hppe_bm_dbg_addr_set(
		a_uint32_t dev_id,
		union bm_dbg_addr_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + BM_DBG_ADDR_ADDRESS,
				value->val);
}

sw_error_t
hppe_bm_dbg_data_get(
		a_uint32_t dev_id,
		union bm_dbg_data_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + BM_DBG_DATA_ADDRESS,
				&value->val);
}

sw_error_t
hppe_bm_dbg_data_set(
		a_uint32_t dev_id,
		union bm_dbg_data_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_fc_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_fc_mode_u *value)
{
	if (index >= PORT_FC_MODE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_FC_MODE_ADDRESS + \
				index * PORT_FC_MODE_INC,
				&value->val);
}
#endif

sw_error_t
hppe_port_fc_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_fc_mode_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_FC_MODE_ADDRESS + \
				index * PORT_FC_MODE_INC,
				value->val);
}

#ifndef IN_BM_MINI
sw_error_t
hppe_port_fc_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_fc_status_u *value)
{
	if (index >= PORT_FC_STATUS_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_FC_STATUS_ADDRESS + \
				index * PORT_FC_STATUS_INC,
				&value->val);
}

sw_error_t
hppe_port_fc_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_fc_status_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_group_id_u *value)
{
	if (index >= PORT_GROUP_ID_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_GROUP_ID_ADDRESS + \
				index * PORT_GROUP_ID_INC,
				&value->val);
}
#endif

sw_error_t
hppe_port_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_group_id_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_GROUP_ID_ADDRESS + \
				index * PORT_GROUP_ID_INC,
				value->val);
}

#ifndef IN_BM_MINI
sw_error_t
hppe_port_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_cnt_u *value)
{
	if (index >= PORT_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_CNT_ADDRESS + \
				index * PORT_CNT_INC,
				&value->val);
}

sw_error_t
hppe_port_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_cnt_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_reacted_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_reacted_cnt_u *value)
{
	if (index >= PORT_REACTED_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_REACTED_CNT_ADDRESS + \
				index * PORT_REACTED_CNT_INC,
				&value->val);
}

sw_error_t
hppe_port_reacted_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_reacted_cnt_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_shared_group_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union shared_group_cnt_u *value)
{
	if (index >= SHARED_GROUP_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + SHARED_GROUP_CNT_ADDRESS + \
				index * SHARED_GROUP_CNT_INC,
				&value->val);
}

sw_error_t
hppe_shared_group_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union shared_group_cnt_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_shared_group_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union shared_group_cfg_u *value)
{
	if (index >= SHARED_GROUP_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + SHARED_GROUP_CFG_ADDRESS + \
				index * SHARED_GROUP_CFG_INC,
				&value->val);
}
#endif

sw_error_t
hppe_shared_group_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union shared_group_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + SHARED_GROUP_CFG_ADDRESS + \
				index * SHARED_GROUP_CFG_INC,
				value->val);
}

#ifndef IN_BM_MINI
sw_error_t
hppe_port_profile_cnt_en_get(
		a_uint32_t dev_id,
		union port_profile_cnt_en_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_PROFILE_CNT_EN_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port_profile_cnt_en_set(
		a_uint32_t dev_id,
		union port_profile_cnt_en_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_PROFILE_CNT_EN_ADDRESS,
				value->val);
}

sw_error_t
hppe_grp_profile_cnt_en_get(
		a_uint32_t dev_id,
		union grp_profile_cnt_en_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + GRP_PROFILE_CNT_EN_ADDRESS,
				&value->val);
}

sw_error_t
hppe_grp_profile_cnt_en_set(
		a_uint32_t dev_id,
		union grp_profile_cnt_en_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + GRP_PROFILE_CNT_EN_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_profile_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_profile_th_cfg_u *value)
{
	if (index >= PORT_PROFILE_TH_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_PROFILE_TH_CFG_ADDRESS + \
				index * PORT_PROFILE_TH_CFG_INC,
				&value->val);
}

sw_error_t
hppe_port_profile_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_profile_th_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_PROFILE_TH_CFG_ADDRESS + \
				index * PORT_PROFILE_TH_CFG_INC,
				value->val);
}

sw_error_t
hppe_react_profile_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union react_profile_th_cfg_u *value)
{
	if (index >= REACT_PROFILE_TH_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + REACT_PROFILE_TH_CFG_ADDRESS + \
				index * REACT_PROFILE_TH_CFG_INC,
				&value->val);
}

sw_error_t
hppe_react_profile_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union react_profile_th_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + REACT_PROFILE_TH_CFG_ADDRESS + \
				index * REACT_PROFILE_TH_CFG_INC,
				value->val);
}

sw_error_t
hppe_grp_profile_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_profile_th_cfg_u *value)
{
	if (index >= GRP_PROFILE_TH_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + GRP_PROFILE_TH_CFG_ADDRESS + \
				index * GRP_PROFILE_TH_CFG_INC,
				&value->val);
}

sw_error_t
hppe_grp_profile_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_profile_th_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + GRP_PROFILE_TH_CFG_ADDRESS + \
				index * GRP_PROFILE_TH_CFG_INC,
				value->val);
}

sw_error_t
hppe_tot_react_profile_th_cfg_get(
		a_uint32_t dev_id,
		union tot_react_profile_th_cfg_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + TOT_REACT_PROFILE_TH_CFG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_tot_react_profile_th_cfg_set(
		a_uint32_t dev_id,
		union tot_react_profile_th_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + TOT_REACT_PROFILE_TH_CFG_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_out_profile_cnt_u *value)
{
	if (index >= PORT_OUT_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_OUT_PROFILE_CNT_ADDRESS + \
				index * PORT_OUT_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_port_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_out_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_OUT_PROFILE_CNT_ADDRESS + \
				index * PORT_OUT_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_port_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_in_profile_cnt_u *value)
{
	if (index >= PORT_IN_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_IN_PROFILE_CNT_ADDRESS + \
				index * PORT_IN_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_port_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_in_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_IN_PROFILE_CNT_ADDRESS + \
				index * PORT_IN_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_react_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union react_out_profile_cnt_u *value)
{
	if (index >= REACT_OUT_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + REACT_OUT_PROFILE_CNT_ADDRESS + \
				index * REACT_OUT_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_react_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union react_out_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + REACT_OUT_PROFILE_CNT_ADDRESS + \
				index * REACT_OUT_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_react_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union react_in_profile_cnt_u *value)
{
	if (index >= REACT_IN_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + REACT_IN_PROFILE_CNT_ADDRESS + \
				index * REACT_IN_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_react_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union react_in_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + REACT_IN_PROFILE_CNT_ADDRESS + \
				index * REACT_IN_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_grp_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_out_profile_cnt_u *value)
{
	if (index >= GRP_OUT_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + GRP_OUT_PROFILE_CNT_ADDRESS + \
				index * GRP_OUT_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_grp_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_out_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + GRP_OUT_PROFILE_CNT_ADDRESS + \
				index * GRP_OUT_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_grp_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_in_profile_cnt_u *value)
{
	if (index >= GRP_IN_PROFILE_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + GRP_IN_PROFILE_CNT_ADDRESS + \
				index * GRP_IN_PROFILE_CNT_INC,
				&value->val);
}

sw_error_t
hppe_grp_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_in_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + GRP_IN_PROFILE_CNT_ADDRESS + \
				index * GRP_IN_PROFILE_CNT_INC,
				value->val);
}

sw_error_t
hppe_tot_react_out_profile_cnt_get(
		a_uint32_t dev_id,
		union tot_react_out_profile_cnt_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + TOT_REACT_OUT_PROFILE_CNT_ADDRESS,
				&value->val);
}

sw_error_t
hppe_tot_react_out_profile_cnt_set(
		a_uint32_t dev_id,
		union tot_react_out_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + TOT_REACT_OUT_PROFILE_CNT_ADDRESS,
				value->val);
}

sw_error_t
hppe_tot_react_in_profile_cnt_get(
		a_uint32_t dev_id,
		union tot_react_in_profile_cnt_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + TOT_REACT_IN_PROFILE_CNT_ADDRESS,
				&value->val);
}

sw_error_t
hppe_tot_react_in_profile_cnt_set(
		a_uint32_t dev_id,
		union tot_react_in_profile_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + TOT_REACT_IN_PROFILE_CNT_ADDRESS,
				value->val);
}
#endif

sw_error_t
hppe_port_fc_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_fc_cfg_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_FC_CFG_ADDRESS + \
				index * PORT_FC_CFG_INC,
				value->val,
				2);
}

sw_error_t
hppe_port_fc_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_fc_cfg_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + PORT_FC_CFG_ADDRESS + \
				index * PORT_FC_CFG_INC,
				value->val,
				2);
}

#ifndef IN_BM_MINI
sw_error_t
hppe_llm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union llm_u *value)
{
	if (index >= LLM_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + LLM_ADDRESS + \
				index * LLM_INC,
				&value->val);
}

sw_error_t
hppe_llm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union llm_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + LLM_ADDRESS + \
				index * LLM_INC,
				value->val);
}

sw_error_t
hppe_rcm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rcm_u *value)
{
	if (index >= RCM_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + RCM_ADDRESS + \
				index * RCM_INC,
				&value->val);
}

sw_error_t
hppe_rcm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rcm_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + RCM_ADDRESS + \
				index * RCM_INC,
				value->val);
}

sw_error_t
hppe_dm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dm_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + DM_ADDRESS + \
				index * DM_INC,
				value->val,
				16);
}

sw_error_t
hppe_dm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dm_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_BM_CSR_BASE_ADDR + DM_ADDRESS + \
				index * DM_INC,
				value->val,
				16);
}

sw_error_t
hppe_fb_fifo_cfg_fb_fifo_thres_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fb_fifo_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fb_fifo_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.fb_fifo_thres;
	return ret;
}

sw_error_t
hppe_fb_fifo_cfg_fb_fifo_thres_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fb_fifo_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fb_fifo_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fb_fifo_thres = value;
	ret = hppe_fb_fifo_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fp_fifo_cfg_fp_fifo_thres_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fp_fifo_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fp_fifo_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.fp_fifo_thres;
	return ret;
}

sw_error_t
hppe_fp_fifo_cfg_fp_fifo_thres_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fp_fifo_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fp_fifo_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fp_fifo_thres = value;
	ret = hppe_fp_fifo_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_deq_fifo_cfg_deq_fifo_thres_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union deq_fifo_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_deq_fifo_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.deq_fifo_thres;
	return ret;
}

sw_error_t
hppe_deq_fifo_cfg_deq_fifo_thres_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union deq_fifo_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_deq_fifo_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.deq_fifo_thres = value;
	ret = hppe_deq_fifo_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_tick_dly_cfg_tick_dly_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tick_dly_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tick_dly_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.tick_dly;
	return ret;
}

sw_error_t
hppe_tick_dly_cfg_tick_dly_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union tick_dly_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tick_dly_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tick_dly = value;
	ret = hppe_tick_dly_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_bm_rsv_0_rsv_0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union bm_rsv_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_bm_rsv_0_get(dev_id, &reg_val);
	*value = reg_val.bf.rsv_0;
	return ret;
}

sw_error_t
hppe_bm_rsv_0_rsv_0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union bm_rsv_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_bm_rsv_0_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rsv_0 = value;
	ret = hppe_bm_rsv_0_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_bm_rsv_1_rsv_1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union bm_rsv_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_bm_rsv_1_get(dev_id, &reg_val);
	*value = reg_val.bf.rsv_1;
	return ret;
}

sw_error_t
hppe_bm_rsv_1_rsv_1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union bm_rsv_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_bm_rsv_1_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rsv_1 = value;
	ret = hppe_bm_rsv_1_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_bm_dbg_addr_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union bm_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_bm_dbg_addr_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_addr;
	return ret;
}

sw_error_t
hppe_bm_dbg_addr_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union bm_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_bm_dbg_addr_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dbg_addr = value;
	ret = hppe_bm_dbg_addr_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_bm_dbg_data_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union bm_dbg_data_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_bm_dbg_data_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_data;
	return ret;
}

sw_error_t
hppe_bm_dbg_data_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_fc_mode_fc_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_mode_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_mode_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fc_en;
	return ret;
}

sw_error_t
hppe_port_fc_mode_fc_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_fc_mode_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_mode_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fc_en = value;
	ret = hppe_port_fc_mode_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_fc_status_port_fc_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_fc_status;
	return ret;
}

sw_error_t
hppe_port_fc_status_port_fc_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_fc_status_port_xon_th_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_xon_th;
	return ret;
}

sw_error_t
hppe_port_fc_status_port_xon_th_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_group_id_port_shared_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_group_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_group_id_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_shared_group_id;
	return ret;
}

sw_error_t
hppe_port_group_id_port_shared_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_group_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_group_id_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_shared_group_id = value;
	ret = hppe_port_group_id_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_cnt_port_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_cnt;
	return ret;
}

sw_error_t
hppe_port_cnt_port_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_reacted_cnt_port_reacted_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_reacted_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_reacted_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_reacted_cnt;
	return ret;
}

sw_error_t
hppe_port_reacted_cnt_port_reacted_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_shared_group_cnt_shared_group_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union shared_group_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_shared_group_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shared_group_cnt;
	return ret;
}

sw_error_t
hppe_shared_group_cnt_shared_group_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_shared_group_cfg_shared_group_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union shared_group_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_shared_group_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shared_group_limit;
	return ret;
}

sw_error_t
hppe_shared_group_cfg_shared_group_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union shared_group_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_shared_group_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.shared_group_limit = value;
	ret = hppe_shared_group_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_8_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_8;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_8_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_8 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_7_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_7;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_7_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_7 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_6_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_6;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_6_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_6 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_2_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_2;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_2_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_2 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_8_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_8;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_8_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_8 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_5_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_5;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_5_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_5 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_12_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_12;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_12_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_12 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_4_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_4;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_4_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_4 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_3_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_3;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_3_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_3 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_10_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_10;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_10_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_10 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_4_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_4;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_4_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_4 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_5_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_5;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_5_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_5 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_14_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_14;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_14_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_14 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_14_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_14;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_14_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_14 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_3_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_3;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_3_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_3 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_1;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_1 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_0;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_0 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_7_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_7;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_7_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_7 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_13_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_13;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_13_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_13 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_6_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_6;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_6_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_6 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_0;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_0 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_13_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_13;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_13_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_13 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_11_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_11;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_11_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_11 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_1;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_1 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_12_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_12;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_12_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_12 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_11_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_11;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_11_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_11 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_10_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_10;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_10_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_10 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_9_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_9;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_9_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_9 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_2_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.port_cnt_en_2;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_port_cnt_en_2_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_cnt_en_2 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_9_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.react_cnt_en_9;
	return ret;
}

sw_error_t
hppe_port_profile_cnt_en_react_cnt_en_9_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_cnt_en_9 = value;
	ret = hppe_port_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_grp_cnt_en_3_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.grp_cnt_en_3;
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_grp_cnt_en_3_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_cnt_en_3 = value;
	ret = hppe_grp_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_tot_rect_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.tot_rect_cnt_en;
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_tot_rect_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tot_rect_cnt_en = value;
	ret = hppe_grp_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_grp_cnt_en_1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.grp_cnt_en_1;
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_grp_cnt_en_1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_cnt_en_1 = value;
	ret = hppe_grp_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_grp_cnt_en_0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.grp_cnt_en_0;
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_grp_cnt_en_0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_cnt_en_0 = value;
	ret = hppe_grp_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_grp_cnt_en_2_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	*value = reg_val.bf.grp_cnt_en_2;
	return ret;
}

sw_error_t
hppe_grp_profile_cnt_en_grp_cnt_en_2_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union grp_profile_cnt_en_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_cnt_en_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_cnt_en_2 = value;
	ret = hppe_grp_profile_cnt_en_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_profile_th_cfg_port_profile_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_profile_th_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_th_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_profile_th_cfg;
	return ret;
}

sw_error_t
hppe_port_profile_th_cfg_port_profile_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_profile_th_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_profile_th_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_profile_th_cfg = value;
	ret = hppe_port_profile_th_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_react_profile_th_cfg_react_profile_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union react_profile_th_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_react_profile_th_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.react_profile_th_cfg;
	return ret;
}

sw_error_t
hppe_react_profile_th_cfg_react_profile_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union react_profile_th_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_react_profile_th_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_profile_th_cfg = value;
	ret = hppe_react_profile_th_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_profile_th_cfg_grp_profile_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union grp_profile_th_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_th_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.grp_profile_th_cfg;
	return ret;
}

sw_error_t
hppe_grp_profile_th_cfg_grp_profile_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union grp_profile_th_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_profile_th_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_profile_th_cfg = value;
	ret = hppe_grp_profile_th_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_tot_react_profile_th_cfg_tot_react_profile_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tot_react_profile_th_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tot_react_profile_th_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.tot_react_profile_th_cfg;
	return ret;
}

sw_error_t
hppe_tot_react_profile_th_cfg_tot_react_profile_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union tot_react_profile_th_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tot_react_profile_th_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tot_react_profile_th_cfg = value;
	ret = hppe_tot_react_profile_th_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_out_profile_cnt_port_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_out_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_out_profile_cnt;
	return ret;
}

sw_error_t
hppe_port_out_profile_cnt_port_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_out_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_out_profile_cnt = value;
	ret = hppe_port_out_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_in_profile_cnt_port_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_in_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_in_profile_cnt;
	return ret;
}

sw_error_t
hppe_port_in_profile_cnt_port_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_in_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_in_profile_cnt = value;
	ret = hppe_port_in_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_react_out_profile_cnt_react_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union react_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_react_out_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.react_out_profile_cnt;
	return ret;
}

sw_error_t
hppe_react_out_profile_cnt_react_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union react_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_react_out_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_out_profile_cnt = value;
	ret = hppe_react_out_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_react_in_profile_cnt_react_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union react_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_react_in_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.react_in_profile_cnt;
	return ret;
}

sw_error_t
hppe_react_in_profile_cnt_react_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union react_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_react_in_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.react_in_profile_cnt = value;
	ret = hppe_react_in_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_out_profile_cnt_grp_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union grp_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_out_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.grp_out_profile_cnt;
	return ret;
}

sw_error_t
hppe_grp_out_profile_cnt_grp_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union grp_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_out_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_out_profile_cnt = value;
	ret = hppe_grp_out_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_grp_in_profile_cnt_grp_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union grp_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_in_profile_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.grp_in_profile_cnt;
	return ret;
}

sw_error_t
hppe_grp_in_profile_cnt_grp_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union grp_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_grp_in_profile_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grp_in_profile_cnt = value;
	ret = hppe_grp_in_profile_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_tot_react_out_profile_cnt_tot_react_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tot_react_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tot_react_out_profile_cnt_get(dev_id, &reg_val);
	*value = reg_val.bf.tot_react_out_profile_cnt;
	return ret;
}

sw_error_t
hppe_tot_react_out_profile_cnt_tot_react_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union tot_react_out_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tot_react_out_profile_cnt_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tot_react_out_profile_cnt = value;
	ret = hppe_tot_react_out_profile_cnt_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_tot_react_in_profile_cnt_tot_react_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tot_react_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tot_react_in_profile_cnt_get(dev_id, &reg_val);
	*value = reg_val.bf.tot_react_in_profile_cnt;
	return ret;
}

sw_error_t
hppe_tot_react_in_profile_cnt_tot_react_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union tot_react_in_profile_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tot_react_in_profile_cnt_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tot_react_in_profile_cnt = value;
	ret = hppe_tot_react_in_profile_cnt_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_pre_alloc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_pre_alloc;
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_pre_alloc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_pre_alloc = value;
	ret = hppe_port_fc_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_resume_offset;
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_resume_offset = value;
	ret = hppe_port_fc_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_shared_dynamic_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_shared_dynamic;
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_shared_dynamic_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_shared_dynamic = value;
	ret = hppe_port_fc_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_shared_weight_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_shared_weight;
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_shared_weight_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_shared_weight = value;
	ret = hppe_port_fc_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_resume_floor_th_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_resume_floor_th;
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_resume_floor_th_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_resume_floor_th = value;
	ret = hppe_port_fc_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_react_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_react_limit;
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_react_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_react_limit = value;
	ret = hppe_port_fc_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_shared_ceiling_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_shared_ceiling_1 << 3 | \
		reg_val.bf.port_shared_ceiling_0;
	return ret;
}

sw_error_t
hppe_port_fc_cfg_port_shared_ceiling_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_fc_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_fc_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_shared_ceiling_1 = value >> 3;
	reg_val.bf.port_shared_ceiling_0 = value & (((a_uint64_t)1<<3)-1);
	ret = hppe_port_fc_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_llm_eop_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union llm_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_llm_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eop;
	return ret;
}

sw_error_t
hppe_llm_eop_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union llm_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_llm_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eop = value;
	ret = hppe_llm_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_llm_nxt_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union llm_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_llm_get(dev_id, index, &reg_val);
	*value = reg_val.bf.nxt_ptr;
	return ret;
}

sw_error_t
hppe_llm_nxt_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union llm_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_llm_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.nxt_ptr = value;
	ret = hppe_llm_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rcm_ref_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rcm_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rcm_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ref_cnt;
	return ret;
}

sw_error_t
hppe_rcm_ref_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rcm_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rcm_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ref_cnt = value;
	ret = hppe_rcm_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_dm_pkt_data_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union dm_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dm_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.pkt_data_1 << 32 | \
		reg_val.bf.pkt_data_0;
	return ret;
}

sw_error_t
hppe_dm_pkt_data_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union dm_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dm_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pkt_data_1 = value >> 32;
	reg_val.bf.pkt_data_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_dm_set(dev_id, index, &reg_val);
	return ret;
}
#endif
