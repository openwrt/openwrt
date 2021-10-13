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
 * @defgroup horus_port_ctrl HORUS_PORT_CONTROL
 * @{
 */
#ifndef _HORUS_PORT_CTRL_H_
#define _HORUS_PORT_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_port_ctrl.h"

    sw_error_t horus_port_ctrl_init(a_uint32_t dev_id);

#ifdef IN_PORTCONTROL
#define HORUS_PORT_CTRL_INIT(rv, dev_id) \
    { \
        rv = horus_port_ctrl_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define HORUS_PORT_CTRL_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    horus_port_duplex_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_port_duplex_t duplex);


    HSL_LOCAL sw_error_t
    horus_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_port_duplex_t * pduplex);


    HSL_LOCAL sw_error_t
    horus_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_port_speed_t speed);


    HSL_LOCAL sw_error_t
    horus_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_port_speed_t * pspeed);


    HSL_LOCAL sw_error_t
    horus_port_autoneg_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t * status);


    HSL_LOCAL sw_error_t
    horus_port_autoneg_enable(a_uint32_t dev_id, fal_port_t port_id);


    HSL_LOCAL sw_error_t
    horus_port_autoneg_restart(a_uint32_t dev_id, fal_port_t port_id);


    HSL_LOCAL sw_error_t
    horus_port_autoneg_adv_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t autoadv);


    HSL_LOCAL sw_error_t
    horus_port_autoneg_adv_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * autoadv);


    HSL_LOCAL sw_error_t
    horus_port_hdr_status_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_hdr_status_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_port_flowctrl_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_flowctrl_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_port_flowctrl_forcemode_set(a_uint32_t dev_id,
                                      fal_port_t port_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_flowctrl_forcemode_get(a_uint32_t dev_id,
                                      fal_port_t port_id,
                                      a_bool_t * enable);

    HSL_LOCAL sw_error_t
    horus_port_powersave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_powersave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);


    HSL_LOCAL sw_error_t
    horus_port_hibernate_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_port_hibernate_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);


    HSL_LOCAL sw_error_t
    horus_port_cdt(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
                   fal_cable_status_t *cable_status, a_uint32_t *cable_len);

#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _HORUS_PORT_CTRL_H_ */
/**
 * @}
 */
