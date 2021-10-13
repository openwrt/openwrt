/*
 * Copyright (c) 2014, 2016-2017, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_trunk FAL_TRUNK
 * @{
 */
#ifndef _FAL_TRUNK_H_
#define _FAL_TRUNK_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"


#define FAL_TRUNK_HASH_KEY_DA              0x1
#define FAL_TRUNK_HASH_KEY_SA              0x2
#define FAL_TRUNK_HASH_KEY_DIP             0x4
#define FAL_TRUNK_HASH_KEY_SIP             0x8
#define FAL_TRUNK_HASH_KEY_SRC_PORT  0x10
#define FAL_TRUNK_HASH_KEY_L4_SRC_PORT           0x20
#define FAL_TRUNK_HASH_KEY_L4_DST_PORT           0x40
#define FAL_TRUNK_HASH_KEY_UDF0             0x80
#define FAL_TRUNK_HASH_KEY_UDF1             0x100
#define FAL_TRUNK_HASH_KEY_UDF2             0x200
#define FAL_TRUNK_HASH_KEY_UDF3             0x400
#define FAL_TRUNK_GROUP_MAX_MEMEBER    4

enum {
	FUNC_TRUNK_GROUP_SET = 0,
	FUNC_TRUNK_GROUP_GET,
	FUNC_TRUNK_HASH_MODE_SET,
	FUNC_TRUNK_HASH_MODE_GET,
	FUNC_TRUNK_FAILOVER_ENABLE,
	FUNC_TRUNK_FAILOVER_STATUS_GET,
};


sw_error_t
fal_trunk_group_set(a_uint32_t dev_id, a_uint32_t trunk_id,
		a_bool_t enable, fal_pbmp_t member);
sw_error_t
fal_trunk_group_get(a_uint32_t dev_id, a_uint32_t trunk_id,
		a_bool_t * enable, fal_pbmp_t * member);
sw_error_t
fal_trunk_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode);

sw_error_t
fal_trunk_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode);

sw_error_t
fal_trunk_manipulate_sa_set(a_uint32_t dev_id, fal_mac_addr_t * addr);

sw_error_t
fal_trunk_manipulate_sa_get(a_uint32_t dev_id, fal_mac_addr_t * addr);

sw_error_t
fal_trunk_failover_enable(a_uint32_t dev_id, a_bool_t failover);

sw_error_t
fal_trunk_failover_status_get(a_uint32_t dev_id, a_bool_t * failover);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_TRUNK_H_ */

/**
 * @}
 */
