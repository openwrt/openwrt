/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
#include "cppe_qos_reg.h"


sw_error_t
cppe_qos_mapping_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union qos_mapping_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L2_BASE_ADDR + QOS_MAPPING_TBL_ADDRESS + \
				index * QOS_MAPPING_TBL_INC,
				value->val,
				2);
}

sw_error_t
cppe_qos_mapping_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union qos_mapping_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L2_BASE_ADDR + QOS_MAPPING_TBL_ADDRESS + \
				index * QOS_MAPPING_TBL_INC,
				value->val,
				2);
}

sw_error_t
cppe_qos_mapping_tbl_int_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_pcp;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_pcp = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_dei;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_dei = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dei_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_dei_en;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dei_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_dei_en = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_dscp_tc_mask_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dscp_tc_mask;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_dscp_tc_mask_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dscp_tc_mask = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dscp_tc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_dscp_tc;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dscp_tc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_dscp_tc = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dp_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_dp_en;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dp_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_dp_en = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_dp;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_dp = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_pri_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_pri_en;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_pri_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_pri_en = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_qos_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qos_res_prec_1 << 1 | \
		reg_val.bf.qos_res_prec_0;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_qos_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qos_res_prec_1 = value >> 1;
	reg_val.bf.qos_res_prec_0 = value & (((a_uint64_t)1<<1)-1);
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_pcp_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_pcp_en;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_pcp_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_pcp_en = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_pri;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_pri = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dscp_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_dscp_en;
	return ret;
}

sw_error_t
cppe_qos_mapping_tbl_int_dscp_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union qos_mapping_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_qos_mapping_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_dscp_en = value;
	ret = cppe_qos_mapping_tbl_set(dev_id, index, &reg_val);
	return ret;
}