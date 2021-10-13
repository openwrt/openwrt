/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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


#ifndef _DESS_VLAN_H_
#define _DESS_VLAN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "fal/fal_vlan.h"

    sw_error_t
    dess_vlan_init(a_uint32_t dev_id);

#ifdef IN_VLAN
#define DESS_VLAN_INIT(rv, dev_id) \
    { \
        rv = dess_vlan_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define DESS_VLAN_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    dess_vlan_entry_append(a_uint32_t dev_id, const fal_vlan_t * vlan_entry);


    HSL_LOCAL sw_error_t
    dess_vlan_create(a_uint32_t dev_id, a_uint32_t vlan_id);


    HSL_LOCAL sw_error_t
    dess_vlan_next(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan);


    HSL_LOCAL sw_error_t
    dess_vlan_find(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan);


    HSL_LOCAL sw_error_t
    dess_vlan_delete(a_uint32_t dev_id, a_uint32_t vlan_id);


    HSL_LOCAL sw_error_t
    dess_vlan_flush(a_uint32_t dev_id);


    HSL_LOCAL sw_error_t
    dess_vlan_fid_set(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t fid);


    HSL_LOCAL sw_error_t
    dess_vlan_fid_get(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t * fid);


    HSL_LOCAL sw_error_t
    dess_vlan_member_add(a_uint32_t dev_id, a_uint32_t vlan_id,
                         fal_port_t port_id, fal_pt_1q_egmode_t port_info);


    HSL_LOCAL sw_error_t
    dess_vlan_member_del(a_uint32_t dev_id, a_uint32_t vlan_id, fal_port_t port_id);


    HSL_LOCAL sw_error_t
    dess_vlan_learning_state_set(a_uint32_t dev_id, a_uint32_t vlan_id,
                                 a_bool_t enable);


    HSL_LOCAL sw_error_t
    dess_vlan_learning_state_get(a_uint32_t dev_id, a_uint32_t vlan_id,
                                 a_bool_t * enable);


#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DESS_VLAN_H_ */

