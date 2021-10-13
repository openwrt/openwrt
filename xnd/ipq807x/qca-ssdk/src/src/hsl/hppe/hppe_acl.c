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
#include "hppe_acl_reg.h"
#include "hppe_acl.h"

sw_error_t
hppe_non_ip_udf0_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + NON_IP_UDF0_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_non_ip_udf0_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + NON_IP_UDF0_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_non_ip_udf1_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + NON_IP_UDF1_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_non_ip_udf1_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + NON_IP_UDF1_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_non_ip_udf2_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + NON_IP_UDF2_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_non_ip_udf2_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + NON_IP_UDF2_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_non_ip_udf3_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + NON_IP_UDF3_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_non_ip_udf3_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + NON_IP_UDF3_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipv4_udf0_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV4_UDF0_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipv4_udf0_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV4_UDF0_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipv4_udf1_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV4_UDF1_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipv4_udf1_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV4_UDF1_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipv4_udf2_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV4_UDF2_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipv4_udf2_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV4_UDF2_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipv4_udf3_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV4_UDF3_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipv4_udf3_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV4_UDF3_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipv6_udf0_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV6_UDF0_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipv6_udf0_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV6_UDF0_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipv6_udf1_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV6_UDF1_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipv6_udf1_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV6_UDF1_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipv6_udf2_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV6_UDF2_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipv6_udf2_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV6_UDF2_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipv6_udf3_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV6_UDF3_CTRL_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipv6_udf3_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPV6_UDF3_CTRL_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_non_ip_udf0_ctrl_reg_udf0_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf0_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_non_ip_udf0_ctrl_reg_udf0_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf0_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_non_ip_udf0_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_non_ip_udf0_ctrl_reg_udf0_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf0_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_non_ip_udf0_ctrl_reg_udf0_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf0_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_non_ip_udf0_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_non_ip_udf1_ctrl_reg_udf1_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf1_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_non_ip_udf1_ctrl_reg_udf1_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf1_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_non_ip_udf1_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_non_ip_udf1_ctrl_reg_udf1_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf1_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_non_ip_udf1_ctrl_reg_udf1_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf1_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_non_ip_udf1_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_non_ip_udf2_ctrl_reg_udf2_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf2_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_non_ip_udf2_ctrl_reg_udf2_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf2_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_non_ip_udf2_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_non_ip_udf2_ctrl_reg_udf2_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf2_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_non_ip_udf2_ctrl_reg_udf2_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf2_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_non_ip_udf2_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_non_ip_udf3_ctrl_reg_udf3_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf3_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_non_ip_udf3_ctrl_reg_udf3_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf3_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_non_ip_udf3_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_non_ip_udf3_ctrl_reg_udf3_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf3_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_non_ip_udf3_ctrl_reg_udf3_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_non_ip_udf3_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_non_ip_udf3_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv4_udf0_ctrl_reg_udf0_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf0_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_ipv4_udf0_ctrl_reg_udf0_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf0_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_ipv4_udf0_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv4_udf0_ctrl_reg_udf0_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf0_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_ipv4_udf0_ctrl_reg_udf0_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf0_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_ipv4_udf0_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv4_udf1_ctrl_reg_udf1_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf1_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_ipv4_udf1_ctrl_reg_udf1_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf1_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_ipv4_udf1_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv4_udf1_ctrl_reg_udf1_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf1_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_ipv4_udf1_ctrl_reg_udf1_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf1_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_ipv4_udf1_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv4_udf2_ctrl_reg_udf2_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf2_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_ipv4_udf2_ctrl_reg_udf2_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf2_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_ipv4_udf2_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv4_udf2_ctrl_reg_udf2_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf2_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_ipv4_udf2_ctrl_reg_udf2_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf2_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_ipv4_udf2_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv4_udf3_ctrl_reg_udf3_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf3_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_ipv4_udf3_ctrl_reg_udf3_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf3_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_ipv4_udf3_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv4_udf3_ctrl_reg_udf3_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf3_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_ipv4_udf3_ctrl_reg_udf3_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv4_udf3_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_ipv4_udf3_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv6_udf0_ctrl_reg_udf0_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf0_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_ipv6_udf0_ctrl_reg_udf0_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf0_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_ipv6_udf0_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv6_udf0_ctrl_reg_udf0_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf0_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_ipv6_udf0_ctrl_reg_udf0_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf0_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_ipv6_udf0_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv6_udf1_ctrl_reg_udf1_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf1_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_ipv6_udf1_ctrl_reg_udf1_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf1_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_ipv6_udf1_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv6_udf1_ctrl_reg_udf1_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf1_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_ipv6_udf1_ctrl_reg_udf1_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf1_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_ipv6_udf1_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv6_udf2_ctrl_reg_udf2_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf2_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_ipv6_udf2_ctrl_reg_udf2_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf2_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_ipv6_udf2_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv6_udf2_ctrl_reg_udf2_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf2_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_ipv6_udf2_ctrl_reg_udf2_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf2_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_ipv6_udf2_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv6_udf3_ctrl_reg_udf3_base_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf3_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_base;
	return ret;
}

sw_error_t
hppe_ipv6_udf3_ctrl_reg_udf3_base_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf3_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_base = value;
	ret = hppe_ipv6_udf3_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipv6_udf3_ctrl_reg_udf3_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf3_ctrl_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf_offset;
	return ret;
}

sw_error_t
hppe_ipv6_udf3_ctrl_reg_udf3_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union udf_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipv6_udf3_ctrl_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf_offset = value;
	ret = hppe_ipv6_udf3_ctrl_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_rule_reg_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_RULE_REG_ADDRESS + \
				index * IPO_RULE_REG_INC,
				value->val,
				3);
}

sw_error_t
hppe_ipo_rule_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_rule_reg_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_RULE_REG_ADDRESS + \
				index * IPO_RULE_REG_INC,
				value->val,
				3);
}

sw_error_t
hppe_ipo_mask_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_mask_reg_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_MASK_REG_ADDRESS + \
				index * IPO_MASK_REG_INC,
				value->val,
				2);
}

sw_error_t
hppe_ipo_mask_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_mask_reg_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_MASK_REG_ADDRESS + \
				index * IPO_MASK_REG_INC,
				value->val,
				2);
}

sw_error_t
hppe_rule_ext_1_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_1_reg_u *value)
{
	if (index >= RULE_EXT_1_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RULE_EXT_1_REG_ADDRESS + \
				index * RULE_EXT_1_REG_INC,
				&value->val);
}

sw_error_t
hppe_rule_ext_1_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_1_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RULE_EXT_1_REG_ADDRESS + \
				index * RULE_EXT_1_REG_INC,
				value->val);
}

sw_error_t
hppe_rule_ext_2_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_2_reg_u *value)
{
	if (index >= RULE_EXT_2_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RULE_EXT_2_REG_ADDRESS + \
				index * RULE_EXT_2_REG_INC,
				&value->val);
}

sw_error_t
hppe_rule_ext_2_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_2_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RULE_EXT_2_REG_ADDRESS + \
				index * RULE_EXT_2_REG_INC,
				value->val);
}

sw_error_t
hppe_rule_ext_4_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_4_reg_u *value)
{
	if (index >= RULE_EXT_4_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RULE_EXT_4_REG_ADDRESS + \
				index * RULE_EXT_4_REG_INC,
				&value->val);
}

sw_error_t
hppe_rule_ext_4_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_4_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RULE_EXT_4_REG_ADDRESS + \
				index * RULE_EXT_4_REG_INC,
				value->val);
}

sw_error_t
hppe_ipo_dbg_addr_reg_get(
		a_uint32_t dev_id,
		union ipo_dbg_addr_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_DBG_ADDR_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipo_dbg_addr_reg_set(
		a_uint32_t dev_id,
		union ipo_dbg_addr_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_DBG_ADDR_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipo_dbg_data_reg_get(
		a_uint32_t dev_id,
		union ipo_dbg_data_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_DBG_DATA_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipo_dbg_data_reg_set(
		a_uint32_t dev_id,
		union ipo_dbg_data_reg_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_ipo_spare_reg_reg_get(
		a_uint32_t dev_id,
		union ipo_spare_reg_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_SPARE_REG_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipo_spare_reg_reg_set(
		a_uint32_t dev_id,
		union ipo_spare_reg_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_SPARE_REG_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_ipo_glb_hit_counter_reg_get(
		a_uint32_t dev_id,
		union ipo_glb_hit_counter_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_GLB_HIT_COUNTER_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipo_glb_hit_counter_reg_set(
		a_uint32_t dev_id,
		union ipo_glb_hit_counter_reg_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_ipo_glb_miss_counter_reg_get(
		a_uint32_t dev_id,
		union ipo_glb_miss_counter_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_GLB_MISS_COUNTER_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipo_glb_miss_counter_reg_set(
		a_uint32_t dev_id,
		union ipo_glb_miss_counter_reg_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_ipo_glb_bypass_counter_reg_get(
		a_uint32_t dev_id,
		union ipo_glb_bypass_counter_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + IPO_GLB_BYPASS_COUNTER_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_ipo_glb_bypass_counter_reg_set(
		a_uint32_t dev_id,
		union ipo_glb_bypass_counter_reg_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_ipo_rule_reg_src_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.src_1 << 3 | \
		reg_val.bf.src_0;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_src_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.src_1 = value >> 3;
	reg_val.bf.src_0 = value & (((a_uint64_t)1<<3)-1);
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_inverse_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.inverse_en;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_inverse_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.inverse_en = value;
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_rule_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rule_type;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_rule_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rule_type = value;
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_src_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.src_type;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_src_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.src_type = value;
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_range_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.range_en;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_range_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.range_en = value;
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_post_routing_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.post_routing_en;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_post_routing_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.post_routing_en = value;
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_fake_mac_header_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fake_mac_header;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_fake_mac_header_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fake_mac_header = value;
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_res_chain_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.res_chain;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_res_chain_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.res_chain = value;
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pri;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pri = value;
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_rule_field_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.rule_field_1 << 32 | \
		reg_val.bf.rule_field_0;
	return ret;
}

sw_error_t
hppe_ipo_rule_reg_rule_field_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union ipo_rule_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_rule_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rule_field_1 = value >> 32;
	reg_val.bf.rule_field_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_ipo_rule_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_mask_reg_maskfield_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union ipo_mask_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_mask_reg_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.maskfield_1 << 32 | \
		reg_val.bf.maskfield_0;
	return ret;
}

sw_error_t
hppe_ipo_mask_reg_maskfield_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union ipo_mask_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_mask_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.maskfield_1 = value >> 32;
	reg_val.bf.maskfield_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_ipo_mask_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rule_ext_1_reg_ext2_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rule_ext_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_1_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ext2_2;
	return ret;
}

sw_error_t
hppe_rule_ext_1_reg_ext2_2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rule_ext_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_1_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ext2_2 = value;
	ret = hppe_rule_ext_1_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rule_ext_1_reg_ext2_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rule_ext_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_1_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ext2_0;
	return ret;
}

sw_error_t
hppe_rule_ext_1_reg_ext2_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rule_ext_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_1_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ext2_0 = value;
	ret = hppe_rule_ext_1_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rule_ext_1_reg_ext2_3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rule_ext_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_1_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ext2_3;
	return ret;
}

sw_error_t
hppe_rule_ext_1_reg_ext2_3_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rule_ext_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_1_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ext2_3 = value;
	ret = hppe_rule_ext_1_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rule_ext_1_reg_ext2_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rule_ext_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_1_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ext2_1;
	return ret;
}

sw_error_t
hppe_rule_ext_1_reg_ext2_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rule_ext_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_1_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ext2_1 = value;
	ret = hppe_rule_ext_1_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rule_ext_2_reg_ext4_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rule_ext_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_2_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ext4_0;
	return ret;
}

sw_error_t
hppe_rule_ext_2_reg_ext4_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rule_ext_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_2_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ext4_0 = value;
	ret = hppe_rule_ext_2_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rule_ext_2_reg_ext4_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rule_ext_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_2_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ext4_1;
	return ret;
}

sw_error_t
hppe_rule_ext_2_reg_ext4_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rule_ext_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_2_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ext4_1 = value;
	ret = hppe_rule_ext_2_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rule_ext_4_reg_ext8_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rule_ext_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_4_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ext8;
	return ret;
}

sw_error_t
hppe_rule_ext_4_reg_ext8_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rule_ext_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rule_ext_4_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ext8 = value;
	ret = hppe_rule_ext_4_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_dbg_addr_reg_ipo_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union ipo_dbg_addr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_dbg_addr_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.ipo_dbg_addr;
	return ret;
}

sw_error_t
hppe_ipo_dbg_addr_reg_ipo_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union ipo_dbg_addr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_dbg_addr_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipo_dbg_addr = value;
	ret = hppe_ipo_dbg_addr_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_dbg_data_reg_ipo_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union ipo_dbg_data_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_dbg_data_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.ipo_dbg_data;
	return ret;
}

sw_error_t
hppe_ipo_dbg_data_reg_ipo_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_ipo_spare_reg_reg_spare_reg_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union ipo_spare_reg_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_spare_reg_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.spare_reg;
	return ret;
}

sw_error_t
hppe_ipo_spare_reg_reg_spare_reg_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union ipo_spare_reg_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_spare_reg_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.spare_reg = value;
	ret = hppe_ipo_spare_reg_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_glb_hit_counter_reg_hit_count_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union ipo_glb_hit_counter_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_glb_hit_counter_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.hit_count;
	return ret;
}

sw_error_t
hppe_ipo_glb_hit_counter_reg_hit_count_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_ipo_glb_miss_counter_reg_miss_count_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union ipo_glb_miss_counter_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_glb_miss_counter_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.miss_count;
	return ret;
}

sw_error_t
hppe_ipo_glb_miss_counter_reg_miss_count_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_ipo_glb_bypass_counter_reg_bypass_count_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union ipo_glb_bypass_counter_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_glb_bypass_counter_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.bypass_count;
	return ret;
}

sw_error_t
hppe_ipo_glb_bypass_counter_reg_bypass_count_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}


sw_error_t
hppe_ipo_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IPO_CNT_TBL_ADDRESS + \
				index * IPO_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_ipo_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IPO_CNT_TBL_ADDRESS + \
				index * IPO_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_ipo_cnt_tbl_hit_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union ipo_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.hit_byte_cnt_1 << 32 | \
		reg_val.bf.hit_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_ipo_cnt_tbl_hit_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union ipo_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hit_byte_cnt_1 = value >> 32;
	reg_val.bf.hit_byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_ipo_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_cnt_tbl_hit_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hit_pkt_cnt;
	return ret;
}

sw_error_t
hppe_ipo_cnt_tbl_hit_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hit_pkt_cnt = value;
	ret = hppe_ipo_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}


sw_error_t
hppe_ipo_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_action_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L2_BASE_ADDR + IPO_ACTION_ADDRESS + \
				index * IPO_ACTION_INC,
				value->val,
				5);
}

sw_error_t
hppe_ipo_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_action_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L2_BASE_ADDR + IPO_ACTION_ADDRESS + \
				index * IPO_ACTION_INC,
				value->val,
				5);
}

sw_error_t
hppe_ipo_action_mirror_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mirror_en;
	return ret;
}

sw_error_t
hppe_ipo_action_mirror_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mirror_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ctag_pcp_1 << 2 | \
		reg_val.bf.ctag_pcp_0;
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ctag_pcp_1 = value >> 2;
	reg_val.bf.ctag_pcp_0 = value & (((a_uint64_t)1<<2)-1);
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_int_dp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_dp_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_int_dp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_dp_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_enqueue_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.enqueue_pri;
	return ret;
}

sw_error_t
hppe_ipo_action_enqueue_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.enqueue_pri = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_stag_pcp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.stag_pcp_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_stag_pcp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.stag_pcp_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_dscp_tc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dscp_tc;
	return ret;
}

sw_error_t
hppe_ipo_action_dscp_tc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dscp_tc = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_cpu_code_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cpu_code_en;
	return ret;
}

sw_error_t
hppe_ipo_action_cpu_code_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cpu_code_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_stag_dei_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.stag_dei_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_stag_dei_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.stag_dei_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ctag_fmt;
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ctag_fmt = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_dest_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dest_info;
	return ret;
}

sw_error_t
hppe_ipo_action_dest_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dest_info = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.svid;
	return ret;
}

sw_error_t
hppe_ipo_action_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.svid = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_dest_info_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dest_info_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_dest_info_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dest_info_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_policer_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.policer_en;
	return ret;
}

sw_error_t
hppe_ipo_action_policer_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.policer_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_int_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.int_dp;
	return ret;
}

sw_error_t
hppe_ipo_action_int_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.int_dp = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_pcp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ctag_pcp_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_pcp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ctag_pcp_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_metadata_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.metadata_en;
	return ret;
}

sw_error_t
hppe_ipo_action_metadata_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.metadata_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_enqueue_pri_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.enqueue_pri_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_enqueue_pri_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.enqueue_pri_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_stag_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.stag_dei;
	return ret;
}

sw_error_t
hppe_ipo_action_stag_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.stag_dei = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fwd_cmd;
	return ret;
}

sw_error_t
hppe_ipo_action_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fwd_cmd = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_bypass_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bypass_bitmap_1 << 14 | \
		reg_val.bf.bypass_bitmap_0;
	return ret;
}

sw_error_t
hppe_ipo_action_bypass_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bypass_bitmap_1 = value >> 14;
	reg_val.bf.bypass_bitmap_0 = value & (((a_uint64_t)1<<14)-1);
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_dei_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ctag_dei_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_dei_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ctag_dei_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_policer_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.policer_index;
	return ret;
}

sw_error_t
hppe_ipo_action_policer_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.policer_index = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ctag_dei;
	return ret;
}

sw_error_t
hppe_ipo_action_ctag_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ctag_dei = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_stag_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.stag_pcp;
	return ret;
}

sw_error_t
hppe_ipo_action_stag_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.stag_pcp = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_syn_toggle_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.syn_toggle;
	return ret;
}

sw_error_t
hppe_ipo_action_syn_toggle_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.syn_toggle = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_service_code_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.service_code_en;
	return ret;
}

sw_error_t
hppe_ipo_action_service_code_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.service_code_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_qid_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qid_en;
	return ret;
}

sw_error_t
hppe_ipo_action_qid_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qid_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_service_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.service_code_1 << 1 | \
		reg_val.bf.service_code_0;
	return ret;
}

sw_error_t
hppe_ipo_action_service_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.service_code_1 = value >> 1;
	reg_val.bf.service_code_0 = value & (((a_uint64_t)1<<1)-1);
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_cvid_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cvid_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_cvid_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cvid_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cvid;
	return ret;
}

sw_error_t
hppe_ipo_action_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cvid = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_svid_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.svid_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_svid_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.svid_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_cpu_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cpu_code;
	return ret;
}

sw_error_t
hppe_ipo_action_cpu_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cpu_code = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_dscp_tc_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dscp_tc_change_en;
	return ret;
}

sw_error_t
hppe_ipo_action_dscp_tc_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dscp_tc_change_en = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_qid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qid;
	return ret;
}

sw_error_t
hppe_ipo_action_qid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qid = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipo_action_stag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.stag_fmt;
	return ret;
}

sw_error_t
hppe_ipo_action_stag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipo_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipo_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.stag_fmt = value;
	ret = hppe_ipo_action_set(dev_id, index, &reg_val);
	return ret;
}

