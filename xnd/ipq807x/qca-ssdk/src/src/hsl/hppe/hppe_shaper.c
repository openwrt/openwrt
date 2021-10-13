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
#include "hppe_shaper_reg.h"
#include "hppe_shaper.h"

sw_error_t
hppe_shp_slot_cfg_l0_get(
		a_uint32_t dev_id,
		union shp_slot_cfg_l0_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + SHP_SLOT_CFG_L0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_shp_slot_cfg_l0_set(
		a_uint32_t dev_id,
		union shp_slot_cfg_l0_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + SHP_SLOT_CFG_L0_ADDRESS,
				value->val);
}

sw_error_t
hppe_shp_slot_cfg_l1_get(
		a_uint32_t dev_id,
		union shp_slot_cfg_l1_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + SHP_SLOT_CFG_L1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_shp_slot_cfg_l1_set(
		a_uint32_t dev_id,
		union shp_slot_cfg_l1_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + SHP_SLOT_CFG_L1_ADDRESS,
				value->val);
}

sw_error_t
hppe_shp_slot_cfg_port_get(
		a_uint32_t dev_id,
		union shp_slot_cfg_port_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + SHP_SLOT_CFG_PORT_ADDRESS,
				&value->val);
}

sw_error_t
hppe_shp_slot_cfg_port_set(
		a_uint32_t dev_id,
		union shp_slot_cfg_port_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + SHP_SLOT_CFG_PORT_ADDRESS,
				value->val);
}

sw_error_t
hppe_l0_shp_credit_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_shp_credit_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_SHP_CREDIT_TBL_ADDRESS + \
				index * L0_SHP_CREDIT_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_l0_shp_credit_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_shp_credit_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_SHP_CREDIT_TBL_ADDRESS + \
				index * L0_SHP_CREDIT_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_l0_shp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_shp_cfg_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_SHP_CFG_TBL_ADDRESS + \
				index * L0_SHP_CFG_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_l0_shp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_shp_cfg_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_SHP_CFG_TBL_ADDRESS + \
				index * L0_SHP_CFG_TBL_INC,
				value->val,
				3);
}

#ifndef IN_SHAPER_MINI
sw_error_t
hppe_l0_comp_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_comp_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_COMP_TBL_ADDRESS + \
				index * L0_COMP_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_l0_comp_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_comp_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif

sw_error_t
hppe_l0_comp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_comp_cfg_tbl_u *value)
{
	if (index >= L0_COMP_CFG_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_COMP_CFG_TBL_ADDRESS + \
				index * L0_COMP_CFG_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_comp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_comp_cfg_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_COMP_CFG_TBL_ADDRESS + \
				index * L0_COMP_CFG_TBL_INC,
				value->val);
}


sw_error_t
hppe_l1_shp_credit_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_shp_credit_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_SHP_CREDIT_TBL_ADDRESS + \
				index * L1_SHP_CREDIT_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_l1_shp_credit_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_shp_credit_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_SHP_CREDIT_TBL_ADDRESS + \
				index * L1_SHP_CREDIT_TBL_INC,
				value->val,
				2);
}

#ifndef IN_SHAPER_MINI
sw_error_t
hppe_l1_shp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_shp_cfg_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_SHP_CFG_TBL_ADDRESS + \
				index * L1_SHP_CFG_TBL_INC,
				value->val,
				3);
}
#endif

sw_error_t
hppe_l1_shp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_shp_cfg_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_SHP_CFG_TBL_ADDRESS + \
				index * L1_SHP_CFG_TBL_INC,
				value->val,
				3);
}


#ifndef IN_SHAPER_MINI
sw_error_t
hppe_l1_comp_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_comp_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_COMP_TBL_ADDRESS + \
				index * L1_COMP_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_l1_comp_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_comp_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif

sw_error_t
hppe_l1_comp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_comp_cfg_tbl_u *value)
{
	if (index >= L1_COMP_CFG_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_COMP_CFG_TBL_ADDRESS + \
				index * L1_COMP_CFG_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_comp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_comp_cfg_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_COMP_CFG_TBL_ADDRESS + \
				index * L1_COMP_CFG_TBL_INC,
				value->val);
}

sw_error_t
hppe_psch_shp_sign_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_shp_sign_tbl_u *value)
{
	if (index >= PSCH_SHP_SIGN_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_SHP_SIGN_TBL_ADDRESS + \
				index * PSCH_SHP_SIGN_TBL_INC,
				&value->val);
}

sw_error_t
hppe_psch_shp_sign_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_shp_sign_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_SHP_SIGN_TBL_ADDRESS + \
				index * PSCH_SHP_SIGN_TBL_INC,
				value->val);
}

sw_error_t
hppe_psch_shp_credit_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_shp_credit_tbl_u *value)
{
	if (index >= PSCH_SHP_CREDIT_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_SHP_CREDIT_TBL_ADDRESS + \
				index * PSCH_SHP_CREDIT_TBL_INC,
				&value->val);
}

sw_error_t
hppe_psch_shp_credit_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_shp_credit_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_SHP_CREDIT_TBL_ADDRESS + \
				index * PSCH_SHP_CREDIT_TBL_INC,
				value->val);
}

sw_error_t
hppe_psch_shp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_shp_cfg_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_SHP_CFG_TBL_ADDRESS + \
				index * PSCH_SHP_CFG_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_psch_shp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_shp_cfg_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_SHP_CFG_TBL_ADDRESS + \
				index * PSCH_SHP_CFG_TBL_INC,
				value->val,
				2);
}

#ifndef IN_SHAPER_MINI
sw_error_t
hppe_psch_comp_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_comp_tbl_u *value)
{
	if (index >= PSCH_COMP_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_COMP_TBL_ADDRESS + \
				index * PSCH_COMP_TBL_INC,
				&value->val);
}

sw_error_t
hppe_psch_comp_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_comp_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif

sw_error_t
hppe_psch_comp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_comp_cfg_tbl_u *value)
{
	if (index >= PSCH_COMP_CFG_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_COMP_CFG_TBL_ADDRESS + \
				index * PSCH_COMP_CFG_TBL_INC,
				&value->val);
}

sw_error_t
hppe_psch_comp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_comp_cfg_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_COMP_CFG_TBL_ADDRESS + \
				index * PSCH_COMP_CFG_TBL_INC,
				value->val);
}

sw_error_t
hppe_ipg_pre_len_cfg_get(
		a_uint32_t dev_id,
		union ipg_pre_len_cfg_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + IPG_PRE_LEN_CFG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipg_pre_len_cfg_set(
		a_uint32_t dev_id,
		union ipg_pre_len_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + IPG_PRE_LEN_CFG_ADDRESS,
				value->val);
}

#ifndef IN_SHAPER_MINI
sw_error_t
hppe_ipg_pre_len_cfg_ipg_pre_len_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union ipg_pre_len_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipg_pre_len_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.ipg_pre_len;
	return ret;
}

sw_error_t
hppe_ipg_pre_len_cfg_ipg_pre_len_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union ipg_pre_len_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipg_pre_len_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipg_pre_len = value;
	ret = hppe_ipg_pre_len_cfg_set(dev_id, &reg_val);
	return ret;
}


sw_error_t
hppe_shp_slot_cfg_l0_l0_shp_slot_time_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union shp_slot_cfg_l0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_shp_slot_cfg_l0_get(dev_id, &reg_val);
	*value = reg_val.bf.l0_shp_slot_time;
	return ret;
}

sw_error_t
hppe_shp_slot_cfg_l0_l0_shp_slot_time_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union shp_slot_cfg_l0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_shp_slot_cfg_l0_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l0_shp_slot_time = value;
	ret = hppe_shp_slot_cfg_l0_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_shp_slot_cfg_l1_l1_shp_slot_time_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union shp_slot_cfg_l1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_shp_slot_cfg_l1_get(dev_id, &reg_val);
	*value = reg_val.bf.l1_shp_slot_time;
	return ret;
}

sw_error_t
hppe_shp_slot_cfg_l1_l1_shp_slot_time_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union shp_slot_cfg_l1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_shp_slot_cfg_l1_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l1_shp_slot_time = value;
	ret = hppe_shp_slot_cfg_l1_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_shp_slot_cfg_port_port_shp_slot_time_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union shp_slot_cfg_port_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_shp_slot_cfg_port_get(dev_id, &reg_val);
	*value = reg_val.bf.port_shp_slot_time;
	return ret;
}

sw_error_t
hppe_shp_slot_cfg_port_port_shp_slot_time_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union shp_slot_cfg_port_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_shp_slot_cfg_port_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_shp_slot_time = value;
	ret = hppe_shp_slot_cfg_port_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_credit_tbl_e_shaper_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_credit_neg;
	return ret;
}

sw_error_t
hppe_l0_shp_credit_tbl_e_shaper_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_credit_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_shaper_credit_neg = value;
	ret = hppe_l0_shp_credit_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_credit_tbl_e_shaper_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_credit_1 << 1 | \
		reg_val.bf.e_shaper_credit_0;
	return ret;
}

sw_error_t
hppe_l0_shp_credit_tbl_e_shaper_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_credit_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_shaper_credit_1 = value >> 1;
	reg_val.bf.e_shaper_credit_0 = value & (((a_uint64_t)1<<1)-1);
	ret = hppe_l0_shp_credit_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_credit_tbl_c_shaper_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_credit;
	return ret;
}

sw_error_t
hppe_l0_shp_credit_tbl_c_shaper_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_credit_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_shaper_credit = value;
	ret = hppe_l0_shp_credit_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_credit_tbl_c_shaper_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_credit_neg;
	return ret;
}

sw_error_t
hppe_l0_shp_credit_tbl_c_shaper_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_credit_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_shaper_credit_neg = value;
	ret = hppe_l0_shp_credit_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_cir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cir;
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_cir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cir = value;
	ret = hppe_l0_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_cf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cf;
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_cf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cf = value;
	ret = hppe_l0_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_meter_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_unit;
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_meter_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_unit = value;
	ret = hppe_l0_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_e_shaper_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_enable;
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_e_shaper_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_shaper_enable = value;
	ret = hppe_l0_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_c_shaper_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_enable;
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_c_shaper_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_shaper_enable = value;
	ret = hppe_l0_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_eir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eir;
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_eir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eir = value;
	ret = hppe_l0_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_token_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.token_unit;
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_token_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.token_unit = value;
	ret = hppe_l0_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_cbs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cbs;
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_cbs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cbs = value;
	ret = hppe_l0_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_ebs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ebs;
	return ret;
}

sw_error_t
hppe_l0_shp_cfg_tbl_ebs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ebs = value;
	ret = hppe_l0_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_c_drr_compensate_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_compensate_byte_cnt_1 << 9 | \
		reg_val.bf.c_drr_compensate_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_c_drr_compensate_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_e_drr_compensate_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_compensate_byte_cnt;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_e_drr_compensate_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_c_shaper_compensate_byte_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_compensate_byte_neg;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_c_shaper_compensate_byte_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_c_shaper_compensate_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_compensate_pkt_cnt;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_c_shaper_compensate_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_c_shaper_compensate_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_compensate_byte_cnt;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_c_shaper_compensate_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_e_shaper_compensate_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_compensate_pkt_cnt;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_e_shaper_compensate_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_e_drr_compensate_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_compensate_pkt_cnt;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_e_drr_compensate_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_e_drr_compensate_byte_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_compensate_byte_neg;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_e_drr_compensate_byte_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_c_drr_compensate_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_compensate_pkt_cnt;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_c_drr_compensate_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_c_drr_compensate_byte_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_compensate_byte_neg;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_c_drr_compensate_byte_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_e_shaper_compensate_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_compensate_byte_cnt;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_e_shaper_compensate_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_tbl_e_shaper_compensate_byte_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_compensate_byte_neg;
	return ret;
}

sw_error_t
hppe_l0_comp_tbl_e_shaper_compensate_byte_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_comp_cfg_tbl_drr_meter_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_meter_len;
	return ret;
}

sw_error_t
hppe_l0_comp_cfg_tbl_drr_meter_len_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_meter_len = value;
	ret = hppe_l0_comp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_comp_cfg_tbl_shaper_meter_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shaper_meter_len;
	return ret;
}

sw_error_t
hppe_l0_comp_cfg_tbl_shaper_meter_len_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_comp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.shaper_meter_len = value;
	ret = hppe_l0_comp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_credit_tbl_e_shaper_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_credit_neg;
	return ret;
}

sw_error_t
hppe_l1_shp_credit_tbl_e_shaper_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_credit_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_shaper_credit_neg = value;
	ret = hppe_l1_shp_credit_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_credit_tbl_e_shaper_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_credit_1 << 1 | \
		reg_val.bf.e_shaper_credit_0;
	return ret;
}

sw_error_t
hppe_l1_shp_credit_tbl_e_shaper_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_credit_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_shaper_credit_1 = value >> 1;
	reg_val.bf.e_shaper_credit_0 = value & (((a_uint64_t)1<<1)-1);
	ret = hppe_l1_shp_credit_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_credit_tbl_c_shaper_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_credit;
	return ret;
}

sw_error_t
hppe_l1_shp_credit_tbl_c_shaper_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_credit_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_shaper_credit = value;
	ret = hppe_l1_shp_credit_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_credit_tbl_c_shaper_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_credit_neg;
	return ret;
}

sw_error_t
hppe_l1_shp_credit_tbl_c_shaper_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_credit_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_shaper_credit_neg = value;
	ret = hppe_l1_shp_credit_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_cir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cir;
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_cir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cir = value;
	ret = hppe_l1_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_cf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cf;
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_cf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cf = value;
	ret = hppe_l1_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_meter_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_unit;
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_meter_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_unit = value;
	ret = hppe_l1_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_e_shaper_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_enable;
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_e_shaper_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_shaper_enable = value;
	ret = hppe_l1_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_c_shaper_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_enable;
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_c_shaper_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_shaper_enable = value;
	ret = hppe_l1_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_eir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eir;
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_eir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eir = value;
	ret = hppe_l1_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_token_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.token_unit;
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_token_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.token_unit = value;
	ret = hppe_l1_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_cbs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cbs;
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_cbs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cbs = value;
	ret = hppe_l1_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_ebs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ebs;
	return ret;
}

sw_error_t
hppe_l1_shp_cfg_tbl_ebs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ebs = value;
	ret = hppe_l1_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_c_drr_compensate_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_compensate_byte_cnt_1 << 9 | \
		reg_val.bf.c_drr_compensate_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_c_drr_compensate_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_e_drr_compensate_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_compensate_byte_cnt;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_e_drr_compensate_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_c_shaper_compensate_byte_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_compensate_byte_neg;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_c_shaper_compensate_byte_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_c_shaper_compensate_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_compensate_pkt_cnt;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_c_shaper_compensate_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_c_shaper_compensate_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_shaper_compensate_byte_cnt;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_c_shaper_compensate_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_e_shaper_compensate_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_compensate_pkt_cnt;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_e_shaper_compensate_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_e_drr_compensate_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_compensate_pkt_cnt;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_e_drr_compensate_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_e_drr_compensate_byte_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_compensate_byte_neg;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_e_drr_compensate_byte_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_c_drr_compensate_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_compensate_pkt_cnt;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_c_drr_compensate_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_c_drr_compensate_byte_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_compensate_byte_neg;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_c_drr_compensate_byte_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_e_shaper_compensate_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_compensate_byte_cnt;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_e_shaper_compensate_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_tbl_e_shaper_compensate_byte_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_shaper_compensate_byte_neg;
	return ret;
}

sw_error_t
hppe_l1_comp_tbl_e_shaper_compensate_byte_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_comp_cfg_tbl_drr_meter_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_meter_len;
	return ret;
}

sw_error_t
hppe_l1_comp_cfg_tbl_drr_meter_len_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_meter_len = value;
	ret = hppe_l1_comp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_comp_cfg_tbl_shaper_meter_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shaper_meter_len;
	return ret;
}

sw_error_t
hppe_l1_comp_cfg_tbl_shaper_meter_len_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_comp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.shaper_meter_len = value;
	ret = hppe_l1_comp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_shp_sign_tbl_shaper_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_shp_sign_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_sign_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shaper_credit_neg;
	return ret;
}

sw_error_t
hppe_psch_shp_sign_tbl_shaper_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_shp_sign_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_sign_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.shaper_credit_neg = value;
	ret = hppe_psch_shp_sign_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_shp_credit_tbl_shaper_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shaper_credit;
	return ret;
}

sw_error_t
hppe_psch_shp_credit_tbl_shaper_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_shp_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_credit_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.shaper_credit = value;
	ret = hppe_psch_shp_credit_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_cir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cir;
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_cir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cir = value;
	ret = hppe_psch_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_meter_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_unit;
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_meter_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_unit = value;
	ret = hppe_psch_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_token_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.token_unit;
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_token_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.token_unit = value;
	ret = hppe_psch_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_cbs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cbs;
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_cbs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cbs = value;
	ret = hppe_psch_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_shaper_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shaper_enable;
	return ret;
}

sw_error_t
hppe_psch_shp_cfg_tbl_shaper_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_shp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_shp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.shaper_enable = value;
	ret = hppe_psch_shp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_comp_tbl_shaper_compensate_byte_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shaper_compensate_byte_neg;
	return ret;
}

sw_error_t
hppe_psch_comp_tbl_shaper_compensate_byte_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_psch_comp_tbl_shaper_compensate_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shaper_compensate_pkt_cnt;
	return ret;
}

sw_error_t
hppe_psch_comp_tbl_shaper_compensate_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_psch_comp_tbl_shaper_compensate_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_comp_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_comp_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shaper_compensate_byte_cnt;
	return ret;
}

sw_error_t
hppe_psch_comp_tbl_shaper_compensate_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_psch_comp_cfg_tbl_shaper_meter_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_comp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.shaper_meter_len;
	return ret;
}

sw_error_t
hppe_psch_comp_cfg_tbl_shaper_meter_len_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_comp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_comp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.shaper_meter_len = value;
	ret = hppe_psch_comp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}
#endif
