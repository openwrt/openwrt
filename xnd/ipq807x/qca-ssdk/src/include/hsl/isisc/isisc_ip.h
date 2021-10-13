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


#ifndef _ISISC_IP_H_
#define _ISISC_IP_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_ip.h"

    sw_error_t isisc_ip_init(a_uint32_t dev_id);

    sw_error_t isisc_ip_reset(a_uint32_t dev_id);

#ifdef IN_IP
#define ISISC_IP_INIT(rv, dev_id) \
    { \
        rv = isisc_ip_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }

#define ISISC_IP_RESET(rv, dev_id) \
    { \
        rv = isisc_ip_reset(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define ISISC_IP_INIT(rv, dev_id)
#define ISISC_IP_RESET(rv, dev_id)
#endif

#ifdef HSL_STANDALONG

    HSL_LOCAL sw_error_t
    isisc_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * host_entry);

    HSL_LOCAL sw_error_t
    isisc_ip_intf_entry_add(a_uint32_t dev_id, fal_intf_mac_entry_t * entry);

    HSL_LOCAL sw_error_t
    isisc_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode,
                     fal_host_entry_t * host_entry);

    HSL_LOCAL sw_error_t
    isisc_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode,
                     fal_host_entry_t * host_entry);

    HSL_LOCAL sw_error_t
    isisc_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode,
                      fal_host_entry_t * host_entry);

    HSL_LOCAL sw_error_t
    isisc_ip_host_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                              a_uint32_t cnt_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    isisc_ip_host_pppoe_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                            a_uint32_t pppoe_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    isisc_ip_pt_arp_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t flags);

    HSL_LOCAL sw_error_t
    isisc_ip_pt_arp_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t * flags);

    HSL_LOCAL sw_error_t
    isisc_ip_arp_learn_set(a_uint32_t dev_id, fal_arp_learn_mode_t mode);

    HSL_LOCAL sw_error_t
    isisc_ip_arp_learn_get(a_uint32_t dev_id, fal_arp_learn_mode_t * mode);

    HSL_LOCAL sw_error_t
    isisc_ip_source_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                             fal_source_guard_mode_t mode);

    HSL_LOCAL sw_error_t
    isisc_ip_source_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                             fal_source_guard_mode_t * mode);

    HSL_LOCAL sw_error_t
    isisc_ip_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);

    HSL_LOCAL sw_error_t
    isisc_ip_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

    HSL_LOCAL sw_error_t
    isisc_ip_arp_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_source_guard_mode_t mode);

    HSL_LOCAL sw_error_t
    isisc_ip_arp_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_source_guard_mode_t * mode);

    HSL_LOCAL sw_error_t
    isisc_arp_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);

    HSL_LOCAL sw_error_t
    isisc_arp_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

    HSL_LOCAL sw_error_t
    isisc_ip_route_status_set(a_uint32_t dev_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    isisc_ip_route_status_get(a_uint32_t dev_id, a_bool_t * enable);

    HSL_LOCAL sw_error_t
    isisc_ip_intf_entry_del(a_uint32_t dev_id, a_uint32_t del_mode,
                           fal_intf_mac_entry_t * entry);

    HSL_LOCAL sw_error_t
    isisc_ip_intf_entry_next(a_uint32_t dev_id, a_uint32_t next_mode,
                            fal_intf_mac_entry_t * entry);

    HSL_LOCAL sw_error_t
    isisc_ip_age_time_set(a_uint32_t dev_id, a_uint32_t * time);

    HSL_LOCAL sw_error_t
    isisc_ip_age_time_get(a_uint32_t dev_id, a_uint32_t * time);

    HSL_LOCAL sw_error_t
    isisc_ip_wcmp_entry_set(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp);

    HSL_LOCAL sw_error_t
    isisc_ip_wcmp_entry_get(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp);

    HSL_LOCAL sw_error_t
    isisc_ip_wcmp_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode);

    HSL_LOCAL sw_error_t
    isisc_ip_wcmp_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode);

#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _ISISC_IP_H_ */
