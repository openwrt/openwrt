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
#include "hppe_trunk_reg.h"
#include "hppe_trunk.h"

sw_error_t
hppe_trunk_hash_field_reg_get(
		a_uint32_t dev_id,
		union trunk_hash_field_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + TRUNK_HASH_FIELD_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_trunk_hash_field_reg_set(
		a_uint32_t dev_id,
		union trunk_hash_field_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + TRUNK_HASH_FIELD_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_trunk_filter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union trunk_filter_u *value)
{
	if (index >= TRUNK_FILTER_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + TRUNK_FILTER_ADDRESS + \
				index * TRUNK_FILTER_INC,
				&value->val);
}

sw_error_t
hppe_trunk_filter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union trunk_filter_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + TRUNK_FILTER_ADDRESS + \
				index * TRUNK_FILTER_INC,
				value->val);
}

sw_error_t
hppe_trunk_member_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union trunk_member_u *value)
{
	if (index >= TRUNK_MEMBER_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + TRUNK_MEMBER_ADDRESS + \
				index * TRUNK_MEMBER_INC,
				&value->val);
}

sw_error_t
hppe_trunk_member_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union trunk_member_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + TRUNK_MEMBER_ADDRESS + \
				index * TRUNK_MEMBER_INC,
				value->val);
}
sw_error_t
hppe_port_trunk_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_trunk_id_u *value)
{
	if (index >= PORT_TRUNK_ID_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_TRUNK_ID_ADDRESS + \
				index * PORT_TRUNK_ID_INC,
				&value->val);
}

sw_error_t
hppe_port_trunk_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_trunk_id_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_TRUNK_ID_ADDRESS + \
				index * PORT_TRUNK_ID_INC,
				value->val);
}

sw_error_t
hppe_trunk_hash_field_reg_udf2_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf2_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_udf2_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf2_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_mac_da_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.mac_da_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_mac_da_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_da_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_src_port_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.src_port_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_src_port_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.src_port_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_udf3_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf3_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_udf3_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf3_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_l4_dst_port_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.l4_dst_port_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_l4_dst_port_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l4_dst_port_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_udf0_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf0_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_udf0_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf0_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_dst_ip_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.dst_ip_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_dst_ip_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dst_ip_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_l4_src_port_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.l4_src_port_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_l4_src_port_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l4_src_port_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_src_ip_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.src_ip_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_src_ip_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.src_ip_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_udf1_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.udf1_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_udf1_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udf1_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_mac_sa_incl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.mac_sa_incl;
	return ret;
}

sw_error_t
hppe_trunk_hash_field_reg_mac_sa_incl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union trunk_hash_field_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_hash_field_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_sa_incl = value;
	ret = hppe_trunk_hash_field_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_filter_mem_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union trunk_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mem_bitmap;
	return ret;
}

sw_error_t
hppe_trunk_filter_mem_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union trunk_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mem_bitmap = value;
	ret = hppe_trunk_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_member_member_2_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	*value = reg_val.bf.member_2_port_id;
	return ret;
}

sw_error_t
hppe_trunk_member_member_2_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.member_2_port_id = value;
	ret = hppe_trunk_member_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_member_member_0_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	*value = reg_val.bf.member_0_port_id;
	return ret;
}

sw_error_t
hppe_trunk_member_member_0_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.member_0_port_id = value;
	ret = hppe_trunk_member_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_member_member_1_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	*value = reg_val.bf.member_1_port_id;
	return ret;
}

sw_error_t
hppe_trunk_member_member_1_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.member_1_port_id = value;
	ret = hppe_trunk_member_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_member_member_6_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	*value = reg_val.bf.member_6_port_id;
	return ret;
}

sw_error_t
hppe_trunk_member_member_6_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.member_6_port_id = value;
	ret = hppe_trunk_member_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_member_member_4_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	*value = reg_val.bf.member_4_port_id;
	return ret;
}

sw_error_t
hppe_trunk_member_member_4_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.member_4_port_id = value;
	ret = hppe_trunk_member_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_member_member_3_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	*value = reg_val.bf.member_3_port_id;
	return ret;
}

sw_error_t
hppe_trunk_member_member_3_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.member_3_port_id = value;
	ret = hppe_trunk_member_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_member_member_5_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	*value = reg_val.bf.member_5_port_id;
	return ret;
}

sw_error_t
hppe_trunk_member_member_5_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.member_5_port_id = value;
	ret = hppe_trunk_member_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_trunk_member_member_7_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	*value = reg_val.bf.member_7_port_id;
	return ret;
}

sw_error_t
hppe_trunk_member_member_7_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union trunk_member_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_trunk_member_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.member_7_port_id = value;
	ret = hppe_trunk_member_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_trunk_id_trunk_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_trunk_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_trunk_id_get(dev_id, index, &reg_val);
	*value = reg_val.bf.trunk_id;
	return ret;
}

sw_error_t
hppe_port_trunk_id_trunk_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_trunk_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_trunk_id_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.trunk_id = value;
	ret = hppe_port_trunk_id_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_trunk_id_trunk_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_trunk_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_trunk_id_get(dev_id, index, &reg_val);
	*value = reg_val.bf.trunk_en;
	return ret;
}

sw_error_t
hppe_port_trunk_id_trunk_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_trunk_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_trunk_id_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.trunk_en = value;
	ret = hppe_port_trunk_id_set(dev_id, index, &reg_val);
	return ret;
}

