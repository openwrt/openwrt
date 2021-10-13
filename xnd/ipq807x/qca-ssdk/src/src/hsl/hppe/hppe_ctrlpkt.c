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
#include "hppe_ctrlpkt_reg.h"
#include "hppe_ctrlpkt.h"

sw_error_t
hppe_ethertype_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ethertype_ctrl_u *value)
{
	if (index >= ETHERTYPE_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + ETHERTYPE_CTRL_ADDRESS + \
				index * ETHERTYPE_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_ethertype_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ethertype_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + ETHERTYPE_CTRL_ADDRESS + \
				index * ETHERTYPE_CTRL_INC,
				value->val);
}

sw_error_t
hppe_app_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union app_ctrl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L2_BASE_ADDR + APP_CTRL_ADDRESS + \
				index * APP_CTRL_INC,
				value->val,
				3);
}

sw_error_t
hppe_app_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union app_ctrl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L2_BASE_ADDR + APP_CTRL_ADDRESS + \
				index * APP_CTRL_INC,
				value->val,
				3);
}

sw_error_t
hppe_ethertype_ctrl_ethertype_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ethertype_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ethertype_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ethertype;
	return ret;
}

sw_error_t
hppe_ethertype_ctrl_ethertype_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ethertype_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ethertype_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ethertype = value;
	ret = hppe_ethertype_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ethertype_ctrl_ethertype_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ethertype_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ethertype_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ethertype_en;
	return ret;
}

sw_error_t
hppe_ethertype_ctrl_ethertype_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ethertype_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ethertype_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ethertype_en = value;
	ret = hppe_ethertype_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_portbitmap_include_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.portbitmap_include;
	return ret;
}

sw_error_t
hppe_app_ctrl_portbitmap_include_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.portbitmap_include = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cmd;
	return ret;
}

sw_error_t
hppe_app_ctrl_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cmd = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_portbitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.portbitmap;
	return ret;
}

sw_error_t
hppe_app_ctrl_portbitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.portbitmap = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_rfdb_index_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rfdb_index_bitmap_1 << 30 | \
		reg_val.bf.rfdb_index_bitmap_0;
	return ret;
}

sw_error_t
hppe_app_ctrl_rfdb_index_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rfdb_index_bitmap_1 = value >> 30;
	reg_val.bf.rfdb_index_bitmap_0 = value & (((a_uint64_t)1<<30)-1);
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_protocol_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.protocol_bitmap;
	return ret;
}

sw_error_t
hppe_app_ctrl_protocol_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.protocol_bitmap = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_in_stg_byp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.in_stg_byp;
	return ret;
}

sw_error_t
hppe_app_ctrl_in_stg_byp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.in_stg_byp = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.valid;
	return ret;
}

sw_error_t
hppe_app_ctrl_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.valid = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_l2_sec_byp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l2_sec_byp;
	return ret;
}

sw_error_t
hppe_app_ctrl_l2_sec_byp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_sec_byp = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_protocol_include_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.protocol_include;
	return ret;
}

sw_error_t
hppe_app_ctrl_protocol_include_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.protocol_include = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_ethertype_include_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ethertype_include;
	return ret;
}

sw_error_t
hppe_app_ctrl_ethertype_include_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ethertype_include = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_sg_byp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.sg_byp;
	return ret;
}

sw_error_t
hppe_app_ctrl_sg_byp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.sg_byp = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_rfdb_include_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rfdb_include;
	return ret;
}

sw_error_t
hppe_app_ctrl_rfdb_include_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rfdb_include = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_ethertype_index_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ethertype_index_bitmap_1 << 2 | \
		reg_val.bf.ethertype_index_bitmap_0;
	return ret;
}

sw_error_t
hppe_app_ctrl_ethertype_index_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ethertype_index_bitmap_1 = value >> 2;
	reg_val.bf.ethertype_index_bitmap_0 = value & (((a_uint64_t)1<<2)-1);
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_app_ctrl_in_vlan_fltr_byp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.in_vlan_fltr_byp;
	return ret;
}

sw_error_t
hppe_app_ctrl_in_vlan_fltr_byp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union app_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_app_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.in_vlan_fltr_byp = value;
	ret = hppe_app_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

