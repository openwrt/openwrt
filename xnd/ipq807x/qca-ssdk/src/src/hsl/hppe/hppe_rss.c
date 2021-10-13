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
#include "hppe_rss_reg.h"
#include "hppe_rss.h"

sw_error_t
hppe_rss_hash_mask_reg_get(
		a_uint32_t dev_id,
		union rss_hash_mask_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_MASK_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_rss_hash_mask_reg_set(
		a_uint32_t dev_id,
		union rss_hash_mask_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_MASK_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_rss_hash_seed_reg_get(
		a_uint32_t dev_id,
		union rss_hash_seed_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_SEED_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_rss_hash_seed_reg_set(
		a_uint32_t dev_id,
		union rss_hash_seed_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_SEED_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_rss_hash_mix_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_mix_reg_u *value)
{
	if (index >= RSS_HASH_MIX_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_MIX_REG_ADDRESS + \
				index * RSS_HASH_MIX_REG_INC,
				&value->val);
}

sw_error_t
hppe_rss_hash_mix_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_mix_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_MIX_REG_ADDRESS + \
				index * RSS_HASH_MIX_REG_INC,
				value->val);
}

sw_error_t
hppe_rss_hash_fin_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_fin_reg_u *value)
{
	if (index >= RSS_HASH_FIN_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_FIN_REG_ADDRESS + \
				index * RSS_HASH_FIN_REG_INC,
				&value->val);
}

sw_error_t
hppe_rss_hash_fin_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_fin_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_FIN_REG_ADDRESS + \
				index * RSS_HASH_FIN_REG_INC,
				value->val);
}

sw_error_t
hppe_rss_hash_mask_ipv4_reg_get(
		a_uint32_t dev_id,
		union rss_hash_mask_ipv4_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_MASK_IPV4_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_rss_hash_mask_ipv4_reg_set(
		a_uint32_t dev_id,
		union rss_hash_mask_ipv4_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_MASK_IPV4_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_rss_hash_seed_ipv4_reg_get(
		a_uint32_t dev_id,
		union rss_hash_seed_ipv4_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_SEED_IPV4_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_rss_hash_seed_ipv4_reg_set(
		a_uint32_t dev_id,
		union rss_hash_seed_ipv4_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_SEED_IPV4_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_rss_hash_mix_ipv4_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_mix_ipv4_reg_u *value)
{
	if (index >= RSS_HASH_MIX_IPV4_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_MIX_IPV4_REG_ADDRESS + \
				index * RSS_HASH_MIX_IPV4_REG_INC,
				&value->val);
}

sw_error_t
hppe_rss_hash_mix_ipv4_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_mix_ipv4_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_MIX_IPV4_REG_ADDRESS + \
				index * RSS_HASH_MIX_IPV4_REG_INC,
				value->val);
}

sw_error_t
hppe_rss_hash_fin_ipv4_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_fin_ipv4_reg_u *value)
{
	if (index >= RSS_HASH_FIN_IPV4_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_FIN_IPV4_REG_ADDRESS + \
				index * RSS_HASH_FIN_IPV4_REG_INC,
				&value->val);
}

sw_error_t
hppe_rss_hash_fin_ipv4_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_fin_ipv4_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPO_CSR_BASE_ADDR + RSS_HASH_FIN_IPV4_REG_ADDRESS + \
				index * RSS_HASH_FIN_IPV4_REG_INC,
				value->val);
}

sw_error_t
hppe_rss_hash_mask_reg_fragment_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union rss_hash_mask_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mask_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.fragment;
	return ret;
}

sw_error_t
hppe_rss_hash_mask_reg_fragment_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union rss_hash_mask_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mask_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fragment = value;
	ret = hppe_rss_hash_mask_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_mask_reg_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union rss_hash_mask_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mask_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.mask;
	return ret;
}

sw_error_t
hppe_rss_hash_mask_reg_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union rss_hash_mask_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mask_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mask = value;
	ret = hppe_rss_hash_mask_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_seed_reg_seed_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union rss_hash_seed_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_seed_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.seed;
	return ret;
}

sw_error_t
hppe_rss_hash_seed_reg_seed_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union rss_hash_seed_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_seed_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.seed = value;
	ret = hppe_rss_hash_seed_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_mix_reg_hash_mix_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rss_hash_mix_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mix_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hash_mix;
	return ret;
}

sw_error_t
hppe_rss_hash_mix_reg_hash_mix_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rss_hash_mix_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mix_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hash_mix = value;
	ret = hppe_rss_hash_mix_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_fin_reg_fin_outer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rss_hash_fin_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_fin_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fin_outer;
	return ret;
}

sw_error_t
hppe_rss_hash_fin_reg_fin_outer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rss_hash_fin_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_fin_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fin_outer = value;
	ret = hppe_rss_hash_fin_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_fin_reg_fin_inner_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rss_hash_fin_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_fin_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fin_inner;
	return ret;
}

sw_error_t
hppe_rss_hash_fin_reg_fin_inner_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rss_hash_fin_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_fin_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fin_inner = value;
	ret = hppe_rss_hash_fin_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_mask_ipv4_reg_fragment_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union rss_hash_mask_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mask_ipv4_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.fragment;
	return ret;
}

sw_error_t
hppe_rss_hash_mask_ipv4_reg_fragment_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union rss_hash_mask_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mask_ipv4_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fragment = value;
	ret = hppe_rss_hash_mask_ipv4_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_mask_ipv4_reg_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union rss_hash_mask_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mask_ipv4_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.mask;
	return ret;
}

sw_error_t
hppe_rss_hash_mask_ipv4_reg_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union rss_hash_mask_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mask_ipv4_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mask = value;
	ret = hppe_rss_hash_mask_ipv4_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_seed_ipv4_reg_seed_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union rss_hash_seed_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_seed_ipv4_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.seed;
	return ret;
}

sw_error_t
hppe_rss_hash_seed_ipv4_reg_seed_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union rss_hash_seed_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_seed_ipv4_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.seed = value;
	ret = hppe_rss_hash_seed_ipv4_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_mix_ipv4_reg_hash_mix_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rss_hash_mix_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mix_ipv4_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hash_mix;
	return ret;
}

sw_error_t
hppe_rss_hash_mix_ipv4_reg_hash_mix_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rss_hash_mix_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_mix_ipv4_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hash_mix = value;
	ret = hppe_rss_hash_mix_ipv4_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_fin_ipv4_reg_fin_outer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rss_hash_fin_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_fin_ipv4_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fin_outer;
	return ret;
}

sw_error_t
hppe_rss_hash_fin_ipv4_reg_fin_outer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rss_hash_fin_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_fin_ipv4_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fin_outer = value;
	ret = hppe_rss_hash_fin_ipv4_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rss_hash_fin_ipv4_reg_fin_inner_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rss_hash_fin_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_fin_ipv4_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fin_inner;
	return ret;
}

sw_error_t
hppe_rss_hash_fin_ipv4_reg_fin_inner_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rss_hash_fin_ipv4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rss_hash_fin_ipv4_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fin_inner = value;
	ret = hppe_rss_hash_fin_ipv4_reg_set(dev_id, index, &reg_val);
	return ret;
}

