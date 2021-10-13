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


#ifndef _ISISC_LEAKY_H_
#define _ISISC_LEAKY_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_leaky.h"

    sw_error_t isisc_leaky_init(a_uint32_t dev_id);

#ifdef IN_LEAKY
#define ISISC_LEAKY_INIT(rv, dev_id) \
    { \
        rv = isisc_leaky_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define ISISC_LEAKY_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    isisc_uc_leaky_mode_set(a_uint32_t dev_id,
                           fal_leaky_ctrl_mode_t ctrl_mode);


    HSL_LOCAL sw_error_t
    isisc_uc_leaky_mode_get(a_uint32_t dev_id,
                           fal_leaky_ctrl_mode_t * ctrl_mode);


    HSL_LOCAL sw_error_t
    isisc_mc_leaky_mode_set(a_uint32_t dev_id,
                           fal_leaky_ctrl_mode_t ctrl_mode);


    HSL_LOCAL sw_error_t
    isisc_mc_leaky_mode_get(a_uint32_t dev_id,
                           fal_leaky_ctrl_mode_t * ctrl_mode);


    HSL_LOCAL sw_error_t
    isisc_port_arp_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable);


    HSL_LOCAL sw_error_t
    isisc_port_arp_leaky_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable);


    HSL_LOCAL sw_error_t
    isisc_port_uc_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable);


    HSL_LOCAL sw_error_t
    isisc_port_uc_leaky_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t * enable);


    HSL_LOCAL sw_error_t
    isisc_port_mc_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable);


    HSL_LOCAL sw_error_t
    isisc_port_mc_leaky_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t * enable);
#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _ISISC_LEAKY_H_ */

