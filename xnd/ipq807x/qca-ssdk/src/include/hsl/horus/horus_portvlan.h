/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
 * @defgroup horus_port_vlan HORUS_PORT_VLAN
 * @{
 */
#ifndef _HORUS_PORTVLAN_H_
#define _HORUS_PORTVLAN_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_portvlan.h"

    sw_error_t horus_portvlan_init(a_uint32_t dev_id);

#ifdef IN_PORTVLAN
#define HORUS_PORTVLAN_INIT(rv, dev_id) \
    { \
        rv = horus_portvlan_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define HORUS_PORTVLAN_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    horus_port_1qmode_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_pt_1qmode_t port_1qmode);


    HSL_LOCAL sw_error_t
    horus_port_1qmode_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_pt_1qmode_t * pport_1qmode);


    HSL_LOCAL sw_error_t
    horus_port_egvlanmode_set(a_uint32_t dev_id, fal_port_t port_id,
                              fal_pt_1q_egmode_t port_egvlanmode);


    HSL_LOCAL sw_error_t
    horus_port_egvlanmode_get(a_uint32_t dev_id, fal_port_t port_id,
                              fal_pt_1q_egmode_t * pport_egvlanmode);


    HSL_LOCAL sw_error_t
    horus_portvlan_member_add(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t mem_port_id);


    HSL_LOCAL sw_error_t
    horus_portvlan_member_del(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t mem_port_id);


    HSL_LOCAL sw_error_t
    horus_portvlan_member_update(a_uint32_t dev_id, fal_port_t port_id,
                                 fal_pbmp_t mem_port_map);


    HSL_LOCAL sw_error_t
    horus_portvlan_member_get(a_uint32_t dev_id, fal_port_t port_id,
                              fal_pbmp_t * mem_port_map);

    HSL_LOCAL sw_error_t
    horus_port_default_vid_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t vid);

    HSL_LOCAL sw_error_t
    horus_port_default_vid_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *vid);

    HSL_LOCAL sw_error_t
    horus_port_force_default_vid_set(a_uint32_t dev_id, fal_port_t port_id,
                                     a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_force_default_vid_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_port_force_portvlan_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_force_portvlan_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_nestvlan_tpid_set(a_uint32_t dev_id, a_uint32_t tpid);


    HSL_LOCAL sw_error_t
    horus_nestvlan_tpid_get(a_uint32_t dev_id, a_uint32_t * tpid);


    HSL_LOCAL sw_error_t
    horus_port_invlan_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                               fal_pt_invlan_mode_t mode);


    HSL_LOCAL sw_error_t
    horus_port_invlan_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                               fal_pt_invlan_mode_t * mode);

    HSL_LOCAL sw_error_t
    horus_port_pri_propagation_set(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_pri_propagation_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t * enable);

    HSL_LOCAL sw_error_t
    horus_qinq_mode_set(a_uint32_t dev_id, fal_qinq_mode_t mode);


    HSL_LOCAL sw_error_t
    horus_qinq_mode_get(a_uint32_t dev_id, fal_qinq_mode_t * mode);


    HSL_LOCAL sw_error_t
    horus_port_qinq_role_set(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t role);


    HSL_LOCAL sw_error_t
    horus_port_qinq_role_get(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t * role);




#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _ATHENA_PORTVLAN_H */
/**
 * @}
 */
