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
#ifndef _HPPE_RSS_H_
#define _HPPE_RSS_H_

#define RSS_HASH_MIX_REG_MAX_ENTRY	11
#define RSS_HASH_FIN_REG_MAX_ENTRY	5
#define RSS_HASH_MIX_IPV4_REG_MAX_ENTRY	5
#define RSS_HASH_FIN_IPV4_REG_MAX_ENTRY	5

sw_error_t
hppe_rss_hash_mask_reg_get(
		a_uint32_t dev_id,
		union rss_hash_mask_reg_u *value);

sw_error_t
hppe_rss_hash_mask_reg_set(
		a_uint32_t dev_id,
		union rss_hash_mask_reg_u *value);

sw_error_t
hppe_rss_hash_seed_reg_get(
		a_uint32_t dev_id,
		union rss_hash_seed_reg_u *value);

sw_error_t
hppe_rss_hash_seed_reg_set(
		a_uint32_t dev_id,
		union rss_hash_seed_reg_u *value);

sw_error_t
hppe_rss_hash_mix_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_mix_reg_u *value);

sw_error_t
hppe_rss_hash_mix_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_mix_reg_u *value);

sw_error_t
hppe_rss_hash_fin_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_fin_reg_u *value);

sw_error_t
hppe_rss_hash_fin_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_fin_reg_u *value);

sw_error_t
hppe_rss_hash_mask_ipv4_reg_get(
		a_uint32_t dev_id,
		union rss_hash_mask_ipv4_reg_u *value);

sw_error_t
hppe_rss_hash_mask_ipv4_reg_set(
		a_uint32_t dev_id,
		union rss_hash_mask_ipv4_reg_u *value);

sw_error_t
hppe_rss_hash_seed_ipv4_reg_get(
		a_uint32_t dev_id,
		union rss_hash_seed_ipv4_reg_u *value);

sw_error_t
hppe_rss_hash_seed_ipv4_reg_set(
		a_uint32_t dev_id,
		union rss_hash_seed_ipv4_reg_u *value);

sw_error_t
hppe_rss_hash_mix_ipv4_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_mix_ipv4_reg_u *value);

sw_error_t
hppe_rss_hash_mix_ipv4_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_mix_ipv4_reg_u *value);

sw_error_t
hppe_rss_hash_fin_ipv4_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_fin_ipv4_reg_u *value);

sw_error_t
hppe_rss_hash_fin_ipv4_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rss_hash_fin_ipv4_reg_u *value);

sw_error_t
hppe_rss_hash_mask_reg_fragment_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_rss_hash_mask_reg_fragment_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_rss_hash_mask_reg_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_rss_hash_mask_reg_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_rss_hash_seed_reg_seed_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_rss_hash_seed_reg_seed_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_rss_hash_mix_reg_hash_mix_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rss_hash_mix_reg_hash_mix_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rss_hash_fin_reg_fin_outer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rss_hash_fin_reg_fin_outer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rss_hash_fin_reg_fin_inner_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rss_hash_fin_reg_fin_inner_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rss_hash_mask_ipv4_reg_fragment_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_rss_hash_mask_ipv4_reg_fragment_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_rss_hash_mask_ipv4_reg_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_rss_hash_mask_ipv4_reg_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_rss_hash_seed_ipv4_reg_seed_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_rss_hash_seed_ipv4_reg_seed_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_rss_hash_mix_ipv4_reg_hash_mix_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rss_hash_mix_ipv4_reg_hash_mix_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rss_hash_fin_ipv4_reg_fin_outer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rss_hash_fin_ipv4_reg_fin_outer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rss_hash_fin_ipv4_reg_fin_inner_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rss_hash_fin_ipv4_reg_fin_inner_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

