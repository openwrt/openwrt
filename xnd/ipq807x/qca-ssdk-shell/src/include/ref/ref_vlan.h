/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup ref_vlan REF_VLAN
 * @{
 */
#ifndef _REF_VLAN_H
#define _REF_VLAN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "sw.h"
#include "fal_type.h"

typedef struct {
	fal_port_t port_id;         /* port id */
	a_uint32_t vid;             /* vlan id */
	a_bool_t   is_wan_port;     /* belong to wan port */
	a_bool_t   valid;           /* valid or not */
} qca_lan_wan_port_info;

typedef struct {
	a_bool_t lan_only_mode;
	qca_lan_wan_port_info v_port_info[SW_MAX_NR_PORT];
} qca_lan_wan_cfg_t;

sw_error_t
qca_lan_wan_cfg_set(a_uint32_t dev_id, qca_lan_wan_cfg_t *lan_wan_cfg);

sw_error_t
qca_lan_wan_cfg_get(a_uint32_t dev_id, qca_lan_wan_cfg_t *lan_wan_cfg);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _REF_VLAN_H */
/**
 * @}
 */
