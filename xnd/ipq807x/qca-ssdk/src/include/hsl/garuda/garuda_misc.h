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



#ifndef _GARUDA_MISC_H_
#define _GARUDA_MISC_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_misc.h"

    sw_error_t garuda_misc_init(a_uint32_t dev_id);

#ifdef IN_MISC
#define GARUDA_MISC_INIT(rv, dev_id) \
    { \
        rv = garuda_misc_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define GARUDA_MISC_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    garuda_arp_status_set(a_uint32_t dev_id, a_bool_t enable);



    HSL_LOCAL sw_error_t
    garuda_arp_status_get(a_uint32_t dev_id, a_bool_t * enable);



    HSL_LOCAL sw_error_t
    garuda_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size);



    HSL_LOCAL sw_error_t
    garuda_frame_max_size_get(a_uint32_t dev_id, a_uint32_t * size);



    HSL_LOCAL sw_error_t
    garuda_port_unk_sa_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                               fal_fwd_cmd_t cmd);



    HSL_LOCAL sw_error_t
    garuda_port_unk_sa_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                               fal_fwd_cmd_t * cmd);



    HSL_LOCAL sw_error_t
    garuda_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t enable);



    HSL_LOCAL sw_error_t
    garuda_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t * enable);



    HSL_LOCAL sw_error_t
    garuda_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t enable);



    HSL_LOCAL sw_error_t
    garuda_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t * enable);



    HSL_LOCAL sw_error_t
    garuda_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable);



    HSL_LOCAL sw_error_t
    garuda_cpu_port_status_get(a_uint32_t dev_id, a_bool_t * enable);



    HSL_LOCAL sw_error_t
    garuda_bc_to_cpu_port_set(a_uint32_t dev_id, a_bool_t enable);



    HSL_LOCAL sw_error_t
    garuda_bc_to_cpu_port_get(a_uint32_t dev_id, a_bool_t * enable);



    HSL_LOCAL sw_error_t
    garuda_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);



    HSL_LOCAL sw_error_t
    garuda_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);


    HSL_LOCAL sw_error_t
    garuda_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable);



    HSL_LOCAL sw_error_t
    garuda_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    garuda_port_dhcp_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);



    HSL_LOCAL sw_error_t
    garuda_port_dhcp_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);
#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _GARUDA_MISC_H_ */
