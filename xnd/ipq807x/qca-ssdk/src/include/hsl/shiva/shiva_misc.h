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
 * @defgroup shiva_misc SHIVA_MISC
 * @{
 */
#ifndef _SHIVA_MISC_H_
#define _SHIVA_MISC_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_misc.h"

    sw_error_t shiva_misc_init(a_uint32_t dev_id);

#ifdef IN_MISC
#define SHIVA_MISC_INIT(rv, dev_id) \
    { \
        rv = shiva_misc_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define SHIVA_MISC_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG

    HSL_LOCAL sw_error_t
    shiva_arp_status_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_arp_status_get(a_uint32_t dev_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    shiva_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size);


    HSL_LOCAL sw_error_t
    shiva_frame_max_size_get(a_uint32_t dev_id, a_uint32_t * size);


    HSL_LOCAL sw_error_t
    shiva_port_unk_sa_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                              fal_fwd_cmd_t cmd);


    HSL_LOCAL sw_error_t
    shiva_port_unk_sa_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                              fal_fwd_cmd_t * cmd);


    HSL_LOCAL sw_error_t
    shiva_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t * enable);


    HSL_LOCAL sw_error_t
    shiva_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t * enable);


    HSL_LOCAL sw_error_t
    shiva_port_bc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_port_bc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable);


    HSL_LOCAL sw_error_t
    shiva_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_cpu_port_status_get(a_uint32_t dev_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    shiva_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);


    HSL_LOCAL sw_error_t
    shiva_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);


    HSL_LOCAL sw_error_t
    shiva_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    shiva_port_dhcp_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_port_dhcp_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    shiva_arp_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);


    HSL_LOCAL sw_error_t
    shiva_arp_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);


    HSL_LOCAL sw_error_t
    shiva_eapol_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);


    HSL_LOCAL sw_error_t
    shiva_eapol_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);


    HSL_LOCAL sw_error_t
    shiva_pppoe_session_add(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t strip_hdr);


    HSL_LOCAL sw_error_t
    shiva_pppoe_session_del(a_uint32_t dev_id, a_uint32_t session_id);


    HSL_LOCAL sw_error_t
    shiva_pppoe_session_get(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t * strip_hdr);

    HSL_LOCAL sw_error_t
    shiva_eapol_status_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    shiva_eapol_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable);

    HSL_LOCAL sw_error_t
    shiva_ripv1_status_set(a_uint32_t dev_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    shiva_ripv1_status_get(a_uint32_t dev_id, a_bool_t * enable);

    HSL_LOCAL sw_error_t
    shiva_loop_check_status_set(a_uint32_t dev_id, fal_loop_check_time_t time, a_bool_t enable);

    HSL_LOCAL sw_error_t
    shiva_loop_check_status_get(a_uint32_t dev_id, fal_loop_check_time_t * time, a_bool_t * enable);

    HSL_LOCAL sw_error_t
    shiva_loop_check_info_get(a_uint32_t dev_id, a_uint32_t * old_port_id, a_uint32_t * new_port_id);


#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _SHIVA_GEN_H_ */
/**
 * @}
 */
