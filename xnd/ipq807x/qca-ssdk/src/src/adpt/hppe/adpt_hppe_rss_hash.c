/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
#include "hppe_rss_reg.h"
#include "hppe_rss.h"
#include "adpt.h"

sw_error_t
adpt_hppe_rss_hash_config_set(a_uint32_t dev_id, fal_rss_hash_mode_t mode,
			fal_rss_hash_config_t * config)
{
	a_uint32_t index;
	union rss_hash_mask_reg_u rss_hash_mask_ipv6 = {0};
	union rss_hash_seed_reg_u rss_hash_seed_ipv6 = {0};
	union rss_hash_mix_reg_u rss_hash_mix_ipv6[11] = {0};
	union rss_hash_fin_reg_u rss_hash_fin_ipv6[5] = {0};
	union rss_hash_mask_ipv4_reg_u rss_hash_mask_ipv4 = {0};
	union rss_hash_seed_ipv4_reg_u rss_hash_seed_ipv4 = {0};
	union rss_hash_mix_ipv4_reg_u rss_hash_mix_ipv4[5] = {0};
	union rss_hash_fin_ipv4_reg_u rss_hash_fin_ipv4[5] = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	if (mode == FAL_RSS_HASH_IPV4V6 || mode == FAL_RSS_HASH_IPV4ONLY)
	{
		rss_hash_mask_ipv4.bf.mask = config->hash_mask & 0x1fffff;
		rss_hash_mask_ipv4.bf.fragment = config->hash_fragment_mode;

		rss_hash_seed_ipv4.bf.seed = config->hash_seed;

		rss_hash_mix_ipv4[0].bf.hash_mix = config->hash_sip_mix & 0x1f;
		rss_hash_mix_ipv4[1].bf.hash_mix = config->hash_dip_mix & 0x1f;
		rss_hash_mix_ipv4[2].bf.hash_mix = config->hash_protocol_mix & 0x1f;
		rss_hash_mix_ipv4[3].bf.hash_mix = config->hash_dport_mix & 0x1f;
		rss_hash_mix_ipv4[4].bf.hash_mix = config->hash_sport_mix & 0x1f;

		rss_hash_fin_ipv4[0].bf.fin_inner = config->hash_fin_inner & 0x1f;
		rss_hash_fin_ipv4[0].bf.fin_outer = config->hash_fin_outer & 0x1f;
		rss_hash_fin_ipv4[1].bf.fin_inner = (config->hash_fin_inner >> 0x5) & 0x1f;
		rss_hash_fin_ipv4[1].bf.fin_outer = (config->hash_fin_outer >> 0x5) & 0x1f;
		rss_hash_fin_ipv4[2].bf.fin_inner = (config->hash_fin_inner >> 0xa) & 0x1f;
		rss_hash_fin_ipv4[2].bf.fin_outer = (config->hash_fin_outer >> 0xa) & 0x1f;
		rss_hash_fin_ipv4[3].bf.fin_inner = (config->hash_fin_inner >> 0xf) & 0x1f;
		rss_hash_fin_ipv4[3].bf.fin_outer = (config->hash_fin_outer >> 0xf) & 0x1f;
		rss_hash_fin_ipv4[4].bf.fin_inner = (config->hash_fin_inner >> 0x14) & 0x1f;
		rss_hash_fin_ipv4[4].bf.fin_outer = (config->hash_fin_outer >> 0x14) & 0x1f;

		SW_RTN_ON_ERROR(hppe_rss_hash_mask_ipv4_reg_set(dev_id, &rss_hash_mask_ipv4));
		SW_RTN_ON_ERROR(hppe_rss_hash_seed_ipv4_reg_set(dev_id, &rss_hash_seed_ipv4));
		for (index = 0; index < 5; index++)
			SW_RTN_ON_ERROR(hppe_rss_hash_mix_ipv4_reg_set(dev_id, index, &rss_hash_mix_ipv4[index]));
		for (index = 0; index < 5; index++)
			SW_RTN_ON_ERROR(hppe_rss_hash_fin_ipv4_reg_set(dev_id, index, &rss_hash_fin_ipv4[index]));
	}

	if (mode == FAL_RSS_HASH_IPV4V6 || mode == FAL_RSS_HASH_IPV6ONLY)
	{
		rss_hash_mask_ipv6.bf.mask = config->hash_mask & 0x1fffff;
		rss_hash_mask_ipv6.bf.fragment = config->hash_fragment_mode;

		rss_hash_seed_ipv6.bf.seed = config->hash_seed;

		rss_hash_mix_ipv6[0].bf.hash_mix = config->hash_sip_mix & 0x1f;
		rss_hash_mix_ipv6[1].bf.hash_mix = (config->hash_sip_mix >> 0x5) & 0x1f;
		rss_hash_mix_ipv6[2].bf.hash_mix = (config->hash_sip_mix >> 0xa) & 0x1f;
		rss_hash_mix_ipv6[3].bf.hash_mix = (config->hash_sip_mix >> 0xf) & 0x1f;

		rss_hash_mix_ipv6[4].bf.hash_mix = config->hash_dip_mix & 0x1f;
		rss_hash_mix_ipv6[5].bf.hash_mix = (config->hash_dip_mix >> 0x5) & 0x1f;
		rss_hash_mix_ipv6[6].bf.hash_mix = (config->hash_dip_mix >> 0xa) & 0x1f;
		rss_hash_mix_ipv6[7].bf.hash_mix = (config->hash_dip_mix >> 0xf) & 0x1f;

		rss_hash_mix_ipv6[8].bf.hash_mix = config->hash_protocol_mix & 0x1f;
		rss_hash_mix_ipv6[9].bf.hash_mix = config->hash_dport_mix & 0x1f;
		rss_hash_mix_ipv6[10].bf.hash_mix = config->hash_sport_mix & 0x1f;

		rss_hash_fin_ipv6[0].bf.fin_inner = config->hash_fin_inner & 0x1f;
		rss_hash_fin_ipv6[0].bf.fin_outer = config->hash_fin_outer & 0x1f;
		rss_hash_fin_ipv6[1].bf.fin_inner = (config->hash_fin_inner >> 0x5) & 0x1f;
		rss_hash_fin_ipv6[1].bf.fin_outer = (config->hash_fin_outer >> 0x5) & 0x1f;
		rss_hash_fin_ipv6[2].bf.fin_inner = (config->hash_fin_inner >> 0xa) & 0x1f;
		rss_hash_fin_ipv6[2].bf.fin_outer = (config->hash_fin_outer >> 0xa) & 0x1f;
		rss_hash_fin_ipv6[3].bf.fin_inner = (config->hash_fin_inner >> 0xf) & 0x1f;
		rss_hash_fin_ipv6[3].bf.fin_outer = (config->hash_fin_outer >> 0xf) & 0x1f;
		rss_hash_fin_ipv6[4].bf.fin_inner = (config->hash_fin_inner >> 0x14) & 0x1f;
		rss_hash_fin_ipv6[4].bf.fin_outer = (config->hash_fin_outer >> 0x14) & 0x1f;

		SW_RTN_ON_ERROR(hppe_rss_hash_mask_reg_set(dev_id, &rss_hash_mask_ipv6));
		SW_RTN_ON_ERROR(hppe_rss_hash_seed_reg_set(dev_id, &rss_hash_seed_ipv6));
		for (index = 0; index < 11; index++)
			SW_RTN_ON_ERROR(hppe_rss_hash_mix_reg_set(dev_id, index, &rss_hash_mix_ipv6[index]));
		for (index = 0; index < 5; index++)
			SW_RTN_ON_ERROR(hppe_rss_hash_fin_reg_set(dev_id, index, &rss_hash_fin_ipv6[index]));
	}

	return SW_OK;
}

sw_error_t
adpt_hppe_rss_hash_config_get(a_uint32_t dev_id, fal_rss_hash_mode_t mode,
			fal_rss_hash_config_t * config)
{
	a_uint32_t index;
	union rss_hash_mask_reg_u rss_hash_mask_ipv6 = {0};
	union rss_hash_seed_reg_u rss_hash_seed_ipv6 = {0};
	union rss_hash_mix_reg_u rss_hash_mix_ipv6[11] = {0};
	union rss_hash_fin_reg_u rss_hash_fin_ipv6[5] = {0};
	union rss_hash_mask_ipv4_reg_u rss_hash_mask_ipv4 = {0};
	union rss_hash_seed_ipv4_reg_u rss_hash_seed_ipv4 = {0};
	union rss_hash_mix_ipv4_reg_u rss_hash_mix_ipv4[5] = {0};
	union rss_hash_fin_ipv4_reg_u rss_hash_fin_ipv4[5] = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	SW_RTN_ON_ERROR(hppe_rss_hash_mask_ipv4_reg_get(dev_id, &rss_hash_mask_ipv4));
	SW_RTN_ON_ERROR(hppe_rss_hash_seed_ipv4_reg_get(dev_id, &rss_hash_seed_ipv4));
	for (index = 0; index < 5; index++)
		SW_RTN_ON_ERROR(hppe_rss_hash_mix_ipv4_reg_get(dev_id, index, &rss_hash_mix_ipv4[index]));
	for (index = 0; index < 5; index++)
		SW_RTN_ON_ERROR(hppe_rss_hash_fin_ipv4_reg_get(dev_id, index, &rss_hash_fin_ipv4[index]));

	SW_RTN_ON_ERROR(hppe_rss_hash_mask_reg_get(dev_id, &rss_hash_mask_ipv6));
	SW_RTN_ON_ERROR(hppe_rss_hash_seed_reg_get(dev_id, &rss_hash_seed_ipv6));
	for (index = 0; index < 11; index++)
		SW_RTN_ON_ERROR(hppe_rss_hash_mix_reg_get(dev_id, index, &rss_hash_mix_ipv6[index]));
	for (index = 0; index < 5; index++)
		SW_RTN_ON_ERROR(hppe_rss_hash_fin_reg_get(dev_id, index, &rss_hash_fin_ipv6[index]));

	if (mode == FAL_RSS_HASH_IPV4ONLY)
	{
		config->hash_mask = rss_hash_mask_ipv4.bf.mask;
		config->hash_fragment_mode = rss_hash_mask_ipv4.bf.fragment;
		config->hash_seed = rss_hash_seed_ipv4.bf.seed;
		config->hash_sip_mix = rss_hash_mix_ipv4[0].bf.hash_mix;
		config->hash_dip_mix = rss_hash_mix_ipv4[1].bf.hash_mix;
		config->hash_protocol_mix = rss_hash_mix_ipv4[2].bf.hash_mix;
		config->hash_dport_mix = rss_hash_mix_ipv4[3].bf.hash_mix;
		config->hash_sport_mix = rss_hash_mix_ipv4[4].bf.hash_mix;
		config->hash_fin_inner = rss_hash_fin_ipv4[0].bf.fin_inner +
				(rss_hash_fin_ipv4[1].bf.fin_inner << 0x5) +
				(rss_hash_fin_ipv4[2].bf.fin_inner << 0xa) +
				(rss_hash_fin_ipv4[3].bf.fin_inner << 0xf) +
				(rss_hash_fin_ipv4[4].bf.fin_inner << 0x14);
		config->hash_fin_outer = rss_hash_fin_ipv4[0].bf.fin_outer +
				(rss_hash_fin_ipv4[1].bf.fin_outer << 0x5) +
				(rss_hash_fin_ipv4[2].bf.fin_outer << 0xa) +
				(rss_hash_fin_ipv4[3].bf.fin_outer << 0xf) +
				(rss_hash_fin_ipv4[4].bf.fin_outer << 0x14);
	}
	else if (mode == FAL_RSS_HASH_IPV6ONLY)
	{
		config->hash_mask = rss_hash_mask_ipv6.bf.mask;
		config->hash_fragment_mode = rss_hash_mask_ipv6.bf.fragment;
		config->hash_seed = rss_hash_seed_ipv6.bf.seed;
		config->hash_sip_mix = rss_hash_mix_ipv6[0].bf.hash_mix +
				(rss_hash_mix_ipv6[1].bf.hash_mix << 0x5) +
				(rss_hash_mix_ipv6[2].bf.hash_mix << 0xa) +
				(rss_hash_mix_ipv6[3].bf.hash_mix << 0xf);
		config->hash_dip_mix = rss_hash_mix_ipv6[4].bf.hash_mix +
				(rss_hash_mix_ipv6[5].bf.hash_mix << 0x5) +
				(rss_hash_mix_ipv6[6].bf.hash_mix << 0xa) +
				(rss_hash_mix_ipv6[7].bf.hash_mix << 0xf);
		config->hash_protocol_mix = rss_hash_mix_ipv6[8].bf.hash_mix;
		config->hash_dport_mix = rss_hash_mix_ipv6[9].bf.hash_mix;
		config->hash_sport_mix = rss_hash_mix_ipv6[10].bf.hash_mix;
		config->hash_fin_inner = rss_hash_fin_ipv6[0].bf.fin_inner +
				(rss_hash_fin_ipv6[1].bf.fin_inner << 0x5) +
				(rss_hash_fin_ipv6[2].bf.fin_inner << 0xa) +
				(rss_hash_fin_ipv6[3].bf.fin_inner << 0xf) +
				(rss_hash_fin_ipv6[4].bf.fin_inner << 0x14);
		config->hash_fin_outer = rss_hash_fin_ipv6[0].bf.fin_outer +
				(rss_hash_fin_ipv6[1].bf.fin_outer << 0x5) +
				(rss_hash_fin_ipv6[2].bf.fin_outer << 0xa) +
				(rss_hash_fin_ipv6[3].bf.fin_outer << 0xf) +
				(rss_hash_fin_ipv6[4].bf.fin_outer << 0x14);
	}
	else
	{
		if ((rss_hash_mask_ipv4.bf.mask == rss_hash_mask_ipv6.bf.mask) &&
			(rss_hash_mask_ipv4.bf.fragment == rss_hash_mask_ipv6.bf.fragment) &&
			(rss_hash_seed_ipv4.bf.seed == rss_hash_seed_ipv6.bf.seed) &&
			(rss_hash_mix_ipv4[0].bf.hash_mix == rss_hash_mix_ipv6[0].bf.hash_mix) &&
			(rss_hash_mix_ipv4[1].bf.hash_mix == rss_hash_mix_ipv6[4].bf.hash_mix) &&
			(rss_hash_mix_ipv4[2].bf.hash_mix == rss_hash_mix_ipv6[8].bf.hash_mix) &&
			(rss_hash_mix_ipv4[3].bf.hash_mix == rss_hash_mix_ipv6[9].bf.hash_mix) &&
			(rss_hash_mix_ipv4[4].bf.hash_mix == rss_hash_mix_ipv6[10].bf.hash_mix) &&
			(rss_hash_fin_ipv4[0].bf.fin_inner == rss_hash_fin_ipv6[0].bf.fin_inner) &&
			(rss_hash_fin_ipv4[1].bf.fin_inner == rss_hash_fin_ipv6[1].bf.fin_inner) &&
			(rss_hash_fin_ipv4[2].bf.fin_inner == rss_hash_fin_ipv6[2].bf.fin_inner) &&
			(rss_hash_fin_ipv4[3].bf.fin_inner == rss_hash_fin_ipv6[3].bf.fin_inner) &&
			(rss_hash_fin_ipv4[4].bf.fin_inner == rss_hash_fin_ipv6[4].bf.fin_inner) &&
			(rss_hash_fin_ipv4[0].bf.fin_outer == rss_hash_fin_ipv6[0].bf.fin_outer) &&
			(rss_hash_fin_ipv4[1].bf.fin_outer == rss_hash_fin_ipv6[1].bf.fin_outer) &&
			(rss_hash_fin_ipv4[2].bf.fin_outer == rss_hash_fin_ipv6[2].bf.fin_outer) &&
			(rss_hash_fin_ipv4[3].bf.fin_outer == rss_hash_fin_ipv6[3].bf.fin_outer) &&
			(rss_hash_fin_ipv4[4].bf.fin_outer == rss_hash_fin_ipv6[4].bf.fin_outer))
		{
			config->hash_mask = rss_hash_mask_ipv6.bf.mask;
			config->hash_fragment_mode = rss_hash_mask_ipv6.bf.fragment;
			config->hash_seed = rss_hash_seed_ipv6.bf.seed;
			config->hash_sip_mix = rss_hash_mix_ipv6[0].bf.hash_mix +
					(rss_hash_mix_ipv6[1].bf.hash_mix << 0x5) +
					(rss_hash_mix_ipv6[2].bf.hash_mix << 0xa) +
					(rss_hash_mix_ipv6[3].bf.hash_mix << 0xf);
			config->hash_dip_mix = rss_hash_mix_ipv6[4].bf.hash_mix +
					(rss_hash_mix_ipv6[5].bf.hash_mix << 0x5) +
					(rss_hash_mix_ipv6[6].bf.hash_mix << 0xa) +
					(rss_hash_mix_ipv6[7].bf.hash_mix << 0xf);
			config->hash_protocol_mix = rss_hash_mix_ipv6[8].bf.hash_mix;
			config->hash_dport_mix = rss_hash_mix_ipv6[9].bf.hash_mix;
			config->hash_sport_mix = rss_hash_mix_ipv6[10].bf.hash_mix;
			config->hash_fin_inner = rss_hash_fin_ipv6[0].bf.fin_inner +
					(rss_hash_fin_ipv6[1].bf.fin_inner << 0x5) +
					(rss_hash_fin_ipv6[2].bf.fin_inner << 0xa) +
					(rss_hash_fin_ipv6[3].bf.fin_inner << 0xf) +
					(rss_hash_fin_ipv6[4].bf.fin_inner << 0x14);
			config->hash_fin_outer = rss_hash_fin_ipv6[0].bf.fin_outer +
					(rss_hash_fin_ipv6[1].bf.fin_outer << 0x5) +
					(rss_hash_fin_ipv6[2].bf.fin_outer << 0xa) +
					(rss_hash_fin_ipv6[3].bf.fin_outer << 0xf) +
					(rss_hash_fin_ipv6[4].bf.fin_outer << 0x14);
		}
		else
			return SW_FAIL;
	}

	return SW_OK;
}

void adpt_hppe_rss_hash_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_rss_hash_func_bitmap = ((1 << FUNC_RSS_HASH_CONFIG_SET) |
						(1 << FUNC_RSS_HASH_CONFIG_GET));

	return;
}

static void adpt_hppe_rss_hash_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_rss_hash_config_set = NULL;
	p_adpt_api->adpt_rss_hash_config_get = NULL;

	return;
}

sw_error_t adpt_hppe_rss_hash_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_rss_hash_func_unregister(dev_id, p_adpt_api);

	if(p_adpt_api->adpt_rss_hash_func_bitmap & (1<<FUNC_RSS_HASH_CONFIG_SET))
		p_adpt_api->adpt_rss_hash_config_set = adpt_hppe_rss_hash_config_set;
	if(p_adpt_api->adpt_rss_hash_func_bitmap & (1<<FUNC_RSS_HASH_CONFIG_GET))
		p_adpt_api->adpt_rss_hash_config_get = adpt_hppe_rss_hash_config_get;

	return SW_OK;
}

/**
 * @}
 */


