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
#include "hppe_stp_reg.h"
#include "hppe_stp.h"

sw_error_t
hppe_cst_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union cst_state_u *value)
{
	if (index >= CST_STATE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + CST_STATE_ADDRESS + \
				index * CST_STATE_INC,
				&value->val);
}

sw_error_t
hppe_cst_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union cst_state_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + CST_STATE_ADDRESS + \
				index * CST_STATE_INC,
				value->val);
}

sw_error_t
hppe_cst_state_port_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cst_state_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_cst_state_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_state;
	return ret;
}

sw_error_t
hppe_cst_state_port_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cst_state_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_cst_state_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_state = value;
	ret = hppe_cst_state_set(dev_id, index, &reg_val);
	return ret;
}


