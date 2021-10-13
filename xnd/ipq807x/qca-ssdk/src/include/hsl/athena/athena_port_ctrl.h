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


#ifndef _ATHENA_PORT_CTRL_H
#define _ATHENA_PORT_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_port_ctrl.h"

    sw_error_t athena_port_ctrl_init(a_uint32_t dev_id);

#ifdef IN_PORTCONTROL
#define ATHENA_PORT_CTRL_INIT(rv, dev_id) \
    { \
        rv = athena_port_ctrl_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define ATHENA_PORT_CTRL_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    athena_port_duplex_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_port_duplex_t duplex);



    HSL_LOCAL sw_error_t
    athena_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_port_duplex_t * pduplex);



    HSL_LOCAL sw_error_t
    athena_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_port_speed_t speed);



    HSL_LOCAL sw_error_t
    athena_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_port_speed_t * pspeed);



    HSL_LOCAL sw_error_t
    athena_port_autoneg_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t * status);



    HSL_LOCAL sw_error_t
    athena_port_autoneg_enable(a_uint32_t dev_id, fal_port_t port_id);



    HSL_LOCAL sw_error_t
    athena_port_autoneg_restart(a_uint32_t dev_id, fal_port_t port_id);



    HSL_LOCAL sw_error_t
    athena_port_autoneg_adv_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t autoadv);



    HSL_LOCAL sw_error_t
    athena_port_autoneg_adv_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t * autoadv);



    HSL_LOCAL sw_error_t
    athena_port_igmps_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable);



    HSL_LOCAL sw_error_t
    athena_port_igmps_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t * enable);


    HSL_LOCAL sw_error_t
    athena_port_powersave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);



    HSL_LOCAL sw_error_t
    athena_port_powersave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);



    HSL_LOCAL sw_error_t
    athena_port_hibernate_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);



    HSL_LOCAL sw_error_t
    athena_port_hibernate_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);


#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _ATHENA_PORT_CTRL_H */
