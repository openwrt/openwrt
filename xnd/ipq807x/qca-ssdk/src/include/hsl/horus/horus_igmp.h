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
 * @defgroup horus_igmp HORUS_IGMP
 * @{
 */
#ifndef _HORUS_IGMP_H_
#define _HORUS_IGMP_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_igmp.h"

    sw_error_t
    horus_igmp_init(a_uint32_t dev_id);

#ifdef IN_IGMP
#define HORUS_IGMP_INIT(rv, dev_id) \
    { \
        rv = horus_igmp_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define HORUS_IGMP_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG

    HSL_LOCAL sw_error_t
    horus_port_igmps_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_igmps_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);


    HSL_LOCAL sw_error_t
    horus_port_igmp_mld_join_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_igmp_mld_join_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_port_igmp_mld_leave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_igmp_mld_leave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_rp_set(a_uint32_t dev_id, fal_pbmp_t pts);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_rp_get(a_uint32_t dev_id, fal_pbmp_t * pts);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_creat_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_creat_get(a_uint32_t dev_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_static_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_static_get(a_uint32_t dev_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_leaky_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_leaky_get(a_uint32_t dev_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_v3_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_v3_get(a_uint32_t dev_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_queue_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t queue);


    HSL_LOCAL sw_error_t
    horus_igmp_mld_entry_queue_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * queue);


#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _HORUS_IGMP_H_ */
/**
 * @}
 */
