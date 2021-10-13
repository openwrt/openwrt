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
#include "hppe_mirror_reg.h"
#include "hppe_mirror.h"

sw_error_t
hppe_mirror_analyzer_get(
		a_uint32_t dev_id,
		union mirror_analyzer_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + MIRROR_ANALYZER_ADDRESS,
				&value->val);
}

sw_error_t
hppe_mirror_analyzer_set(
		a_uint32_t dev_id,
		union mirror_analyzer_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + MIRROR_ANALYZER_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_mirror_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_mirror_u *value)
{
	if (index >= PORT_MIRROR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_MIRROR_ADDRESS + \
				index * PORT_MIRROR_INC,
				&value->val);
}

sw_error_t
hppe_port_mirror_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_mirror_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_MIRROR_ADDRESS + \
				index * PORT_MIRROR_INC,
				value->val);
}

sw_error_t
hppe_mirror_analyzer_in_analyzer_port_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union mirror_analyzer_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mirror_analyzer_get(dev_id, &reg_val);
	*value = reg_val.bf.in_analyzer_port;
	return ret;
}

sw_error_t
hppe_mirror_analyzer_in_analyzer_port_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union mirror_analyzer_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mirror_analyzer_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.in_analyzer_port = value;
	ret = hppe_mirror_analyzer_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_mirror_analyzer_eg_analyzer_port_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union mirror_analyzer_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mirror_analyzer_get(dev_id, &reg_val);
	*value = reg_val.bf.eg_analyzer_port;
	return ret;
}

sw_error_t
hppe_mirror_analyzer_eg_analyzer_port_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union mirror_analyzer_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mirror_analyzer_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eg_analyzer_port = value;
	ret = hppe_mirror_analyzer_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_mirror_in_mirr_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_mirror_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mirror_get(dev_id, index, &reg_val);
	*value = reg_val.bf.in_mirr_en;
	return ret;
}

sw_error_t
hppe_port_mirror_in_mirr_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_mirror_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mirror_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.in_mirr_en = value;
	ret = hppe_port_mirror_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_mirror_eg_mirr_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_mirror_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mirror_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eg_mirr_en;
	return ret;
}

sw_error_t
hppe_port_mirror_eg_mirr_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_mirror_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mirror_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eg_mirr_en = value;
	ret = hppe_port_mirror_set(dev_id, index, &reg_val);
	return ret;
}
