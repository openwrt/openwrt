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
 * @defgroup fal_gen FAL_RSS_HASH
 * @{
 */
#ifndef _FAL_RSS_HASH_H_
#define _FAL_RSS_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

typedef enum
{
	FAL_RSS_HASH_IPV4V6 = 0,
	FAL_RSS_HASH_IPV4ONLY = 1,
	FAL_RSS_HASH_IPV6ONLY = 2,
} fal_rss_hash_mode_t;

typedef struct
{
	a_uint32_t 	hash_mask; /* final hash value bits */
	a_bool_t	hash_fragment_mode; /* enable fragment mode or not */
	a_uint32_t	hash_seed; /* rss hash seed value */
	a_uint32_t	hash_sip_mix; /* source ip hash mix */
	a_uint32_t	hash_dip_mix; /* dest ip hash mix */
	a_uint8_t	hash_protocol_mix; /* L4 protocol hash mix */
	a_uint8_t	hash_sport_mix; /* L4 source port hash mix */
	a_uint8_t	hash_dport_mix; /* L4 dest port hash mix */
	a_uint32_t	hash_fin_inner; /* hash fin inner mix */
	a_uint32_t	hash_fin_outer; /* hash fin outer mix */
} fal_rss_hash_config_t;

enum {
	FUNC_RSS_HASH_CONFIG_SET = 0,
	FUNC_RSS_HASH_CONFIG_GET,
};

sw_error_t
fal_rss_hash_config_set(a_uint32_t dev_id, fal_rss_hash_mode_t mode, fal_rss_hash_config_t * config);

sw_error_t
fal_rss_hash_config_get(a_uint32_t dev_id, fal_rss_hash_mode_t mode, fal_rss_hash_config_t * config);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_RSS_HASH_H_ */
/**
 * @}
 */

