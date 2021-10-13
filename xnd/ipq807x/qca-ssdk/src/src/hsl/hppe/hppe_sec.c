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
#include "hppe_sec_reg.h"
#include "hppe_sec.h"

sw_error_t
hppe_l3_exception_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exception_cmd_u *value)
{
	if (index >= L3_EXCEPTION_CMD_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXCEPTION_CMD_ADDRESS + \
				index * L3_EXCEPTION_CMD_INC,
				&value->val);
}

sw_error_t
hppe_l3_exception_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exception_cmd_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXCEPTION_CMD_ADDRESS + \
				index * L3_EXCEPTION_CMD_INC,
				value->val);
}

sw_error_t
hppe_l3_exp_l3_only_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l3_only_ctrl_u *value)
{
	if (index >= L3_EXP_L3_ONLY_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_L3_ONLY_CTRL_ADDRESS + \
				index * L3_EXP_L3_ONLY_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_l3_exp_l3_only_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l3_only_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_L3_ONLY_CTRL_ADDRESS + \
				index * L3_EXP_L3_ONLY_CTRL_INC,
				value->val);
}

sw_error_t
hppe_l3_exp_l2_only_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l2_only_ctrl_u *value)
{
	if (index >= L3_EXP_L2_ONLY_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_L2_ONLY_CTRL_ADDRESS + \
				index * L3_EXP_L2_ONLY_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_l3_exp_l2_only_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l2_only_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_L2_ONLY_CTRL_ADDRESS + \
				index * L3_EXP_L2_ONLY_CTRL_INC,
				value->val);
}

sw_error_t
hppe_l3_exp_l2_flow_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l2_flow_ctrl_u *value)
{
	if (index >= L3_EXP_L2_FLOW_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_L2_FLOW_CTRL_ADDRESS + \
				index * L3_EXP_L2_FLOW_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_l3_exp_l2_flow_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l2_flow_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_L2_FLOW_CTRL_ADDRESS + \
				index * L3_EXP_L2_FLOW_CTRL_INC,
				value->val);
}

sw_error_t
hppe_l3_exp_l3_flow_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l3_flow_ctrl_u *value)
{
	if (index >= L3_EXP_L3_FLOW_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_L3_FLOW_CTRL_ADDRESS + \
				index * L3_EXP_L3_FLOW_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_l3_exp_l3_flow_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l3_flow_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_L3_FLOW_CTRL_ADDRESS + \
				index * L3_EXP_L3_FLOW_CTRL_INC,
				value->val);
}

sw_error_t
hppe_l3_exp_multicast_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_multicast_ctrl_u *value)
{
	if (index >= L3_EXP_MULTICAST_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_MULTICAST_CTRL_ADDRESS + \
				index * L3_EXP_MULTICAST_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_l3_exp_multicast_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_multicast_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_EXP_MULTICAST_CTRL_ADDRESS + \
				index * L3_EXP_MULTICAST_CTRL_INC,
				value->val);
}

sw_error_t
hppe_l3_exception_cmd_l3_excep_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_exception_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exception_cmd_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l3_excep_cmd;
	return ret;
}

sw_error_t
hppe_l3_exception_cmd_l3_excep_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_exception_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exception_cmd_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_excep_cmd = value;
	ret = hppe_l3_exception_cmd_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_exception_cmd_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_exception_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exception_cmd_get(dev_id, index, &reg_val);
	*value = reg_val.bf.de_acce;
	return ret;
}

sw_error_t
hppe_l3_exception_cmd_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_exception_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exception_cmd_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.de_acce = value;
	ret = hppe_l3_exception_cmd_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_exp_l3_only_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_exp_l3_only_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_l3_only_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.excep_en;
	return ret;
}

sw_error_t
hppe_l3_exp_l3_only_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_exp_l3_only_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_l3_only_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.excep_en = value;
	ret = hppe_l3_exp_l3_only_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_exp_l2_only_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_exp_l2_only_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_l2_only_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.excep_en;
	return ret;
}

sw_error_t
hppe_l3_exp_l2_only_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_exp_l2_only_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_l2_only_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.excep_en = value;
	ret = hppe_l3_exp_l2_only_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_exp_l2_flow_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_exp_l2_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_l2_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.excep_en;
	return ret;
}

sw_error_t
hppe_l3_exp_l2_flow_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_exp_l2_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_l2_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.excep_en = value;
	ret = hppe_l3_exp_l2_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_exp_l3_flow_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_exp_l3_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_l3_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.excep_en;
	return ret;
}

sw_error_t
hppe_l3_exp_l3_flow_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_exp_l3_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_l3_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.excep_en = value;
	ret = hppe_l3_exp_l3_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_exp_multicast_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_exp_multicast_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_multicast_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.excep_en;
	return ret;
}

sw_error_t
hppe_l3_exp_multicast_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_exp_multicast_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exp_multicast_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.excep_en = value;
	ret = hppe_l3_exp_multicast_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_get(
		a_uint32_t dev_id,
		union l3_exception_parsing_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + L3_EXCEPTION_PARSING_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_set(
		a_uint32_t dev_id,
		union l3_exception_parsing_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + L3_EXCEPTION_PARSING_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_get(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_0_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + L4_EXCEPTION_PARSING_CTRL_0_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_set(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_0_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + L4_EXCEPTION_PARSING_CTRL_0_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_get(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_1_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + L4_EXCEPTION_PARSING_CTRL_1_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_set(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_1_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + L4_EXCEPTION_PARSING_CTRL_1_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_get(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_2_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + L4_EXCEPTION_PARSING_CTRL_2_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_set(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_2_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + L4_EXCEPTION_PARSING_CTRL_2_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_get(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_3_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + L4_EXCEPTION_PARSING_CTRL_3_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_set(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_3_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + L4_EXCEPTION_PARSING_CTRL_3_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_small_hop_limit_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_exception_parsing_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exception_parsing_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.small_hop_limit;
	return ret;
}

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_small_hop_limit_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_exception_parsing_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exception_parsing_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.small_hop_limit = value;
	ret = hppe_l3_exception_parsing_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_small_ttl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_exception_parsing_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exception_parsing_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.small_ttl;
	return ret;
}

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_small_ttl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_exception_parsing_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_exception_parsing_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.small_ttl = value;
	ret = hppe_l3_exception_parsing_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_0_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags0;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_0_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags0 = value;
	ret = hppe_l4_exception_parsing_ctrl_0_reg_set(dev_id, &reg_val);
	return ret;
}
sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags0_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_0_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags0_mask;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags0_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_0_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags0_mask = value;
	ret = hppe_l4_exception_parsing_ctrl_0_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_0_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags1;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_0_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags1 = value;
	ret = hppe_l4_exception_parsing_ctrl_0_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags1_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_0_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags1_mask;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags1_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_0_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags1_mask = value;
	ret = hppe_l4_exception_parsing_ctrl_0_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags2_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_1_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags2;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags2_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_1_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags2 = value;
	ret = hppe_l4_exception_parsing_ctrl_1_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags2_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_1_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags2_mask;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags2_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_1_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags2_mask = value;
	ret = hppe_l4_exception_parsing_ctrl_1_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags3_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_1_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags3;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags3_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_1_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags3 = value;
	ret = hppe_l4_exception_parsing_ctrl_1_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags3_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_1_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags3_mask;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags3_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_1_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags3_mask = value;
	ret = hppe_l4_exception_parsing_ctrl_1_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags4_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_2_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags4;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags4_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_2_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags4 = value;
	ret = hppe_l4_exception_parsing_ctrl_2_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags4_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_2_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags4_mask;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags4_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_2_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags4_mask = value;
	ret = hppe_l4_exception_parsing_ctrl_2_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags5_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_2_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags5;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags5_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_2_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags5 = value;
	ret = hppe_l4_exception_parsing_ctrl_2_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags5_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_2_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags5_mask;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags5_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_2_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags5_mask = value;
	ret = hppe_l4_exception_parsing_ctrl_2_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags6_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_3_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags6;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags6_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_3_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags6 = value;
	ret = hppe_l4_exception_parsing_ctrl_3_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags6_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_3_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags6_mask;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags6_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_3_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags6_mask = value;
	ret = hppe_l4_exception_parsing_ctrl_3_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags7_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_3_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags7;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags7_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_3_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags7 = value;
	ret = hppe_l4_exception_parsing_ctrl_3_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags7_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l4_exception_parsing_ctrl_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_3_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.tcp_flags7_mask;
	return ret;
}

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags7_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l4_exception_parsing_ctrl_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l4_exception_parsing_ctrl_3_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tcp_flags7_mask = value;
	ret = hppe_l4_exception_parsing_ctrl_3_reg_set(dev_id, &reg_val);
	return ret;
}

