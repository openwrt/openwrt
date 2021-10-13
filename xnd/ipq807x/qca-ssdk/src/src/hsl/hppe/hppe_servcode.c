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
#include "hppe_servcode_reg.h"
#include "hppe_servcode.h"

sw_error_t
hppe_service_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union service_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + SERVICE_TBL_ADDRESS + \
				index * SERVICE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_service_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union service_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + SERVICE_TBL_ADDRESS + \
				index * SERVICE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_service_tbl_rx_counting_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_counting_en;
	return ret;
}

sw_error_t
hppe_service_tbl_rx_counting_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_counting_en = value;
	ret = hppe_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_service_tbl_bypass_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bypass_bitmap;
	return ret;
}

sw_error_t
hppe_service_tbl_bypass_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bypass_bitmap = value;
	ret = hppe_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_l2_service_tbl_u *value)
{
	if (index >= IN_L2_SERVICE_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + IN_L2_SERVICE_TBL_ADDRESS + \
				index * IN_L2_SERVICE_TBL_INC,
				&value->val);
}

sw_error_t
hppe_in_l2_service_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_l2_service_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + IN_L2_SERVICE_TBL_ADDRESS + \
				index * IN_L2_SERVICE_TBL_INC,
				value->val);
}

sw_error_t
hppe_in_l2_service_tbl_direction_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.direction;
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_direction_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.direction = value;
	ret = hppe_in_l2_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_rx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_cnt_en;
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_rx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_cnt_en = value;
	ret = hppe_in_l2_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_tx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_cnt_en;
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_tx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_cnt_en = value;
	ret = hppe_in_l2_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_bypass_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bypass_bitmap;
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_bypass_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bypass_bitmap = value;
	ret = hppe_in_l2_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_dst_port_id_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dst_port_id_valid;
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_dst_port_id_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dst_port_id_valid = value;
	ret = hppe_in_l2_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_dst_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dst_port_id;
	return ret;
}

sw_error_t
hppe_in_l2_service_tbl_dst_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l2_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l2_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dst_port_id = value;
	ret = hppe_in_l2_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_service_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_service_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_SERVICE_TBL_ADDRESS + \
				index * EG_SERVICE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_eg_service_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_service_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_SERVICE_TBL_ADDRESS + \
				index * EG_SERVICE_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_eg_service_tbl_next_service_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_service_code;
	return ret;
}

sw_error_t
hppe_eg_service_tbl_next_service_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.next_service_code = value;
	ret = hppe_eg_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_service_tbl_tx_counting_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_counting_en;
	return ret;
}

sw_error_t
hppe_eg_service_tbl_tx_counting_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_counting_en = value;
	ret = hppe_eg_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_service_tbl_field_update_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.field_update_action;
	return ret;
}

sw_error_t
hppe_eg_service_tbl_field_update_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.field_update_action = value;
	ret = hppe_eg_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_service_tbl_offset_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.offset_sel;
	return ret;
}

sw_error_t
hppe_eg_service_tbl_offset_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.offset_sel = value;
	ret = hppe_eg_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_service_tbl_hw_services_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hw_services;
	return ret;
}

sw_error_t
hppe_eg_service_tbl_hw_services_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_service_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_service_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hw_services = value;
	ret = hppe_eg_service_tbl_set(dev_id, index, &reg_val);
	return ret;
}