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


#ifndef _DESS_NAT_H_
#define _DESS_NAT_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_nat.h"

    sw_error_t dess_nat_init(a_uint32_t dev_id);

    sw_error_t dess_nat_reset(a_uint32_t dev_id);

#ifdef IN_NAT
#define DESS_NAT_INIT(rv, dev_id) \
    { \
        rv = dess_nat_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }

#define DESS_NAT_RESET(rv, dev_id) \
    { \
        rv = dess_nat_reset(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define DESS_NAT_INIT(rv, dev_id)
#define DESS_NAT_RESET(rv, dev_id)
#endif

#ifdef HSL_STANDALONG

    HSL_LOCAL sw_error_t
    dess_napt_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry);

    HSL_LOCAL sw_error_t
    dess_nat_pub_addr_add(a_uint32_t dev_id, fal_nat_pub_addr_t * entry);

    HSL_LOCAL sw_error_t
    dess_napt_next(a_uint32_t dev_id, a_uint32_t next_mode,
                   fal_napt_entry_t * napt_entry);

    HSL_LOCAL sw_error_t
    dess_nat_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr);

    HSL_LOCAL sw_error_t
    dess_napt_del(a_uint32_t dev_id, a_uint32_t del_mode,
                  fal_napt_entry_t * napt_entry);

    HSL_LOCAL sw_error_t
    dess_nat_del(a_uint32_t dev_id, a_uint32_t del_mode,
                 fal_nat_entry_t * nat_entry);

    HSL_LOCAL sw_error_t
    dess_nat_pub_addr_del(a_uint32_t dev_id, a_uint32_t del_mode,
                          fal_nat_pub_addr_t * entry);

    HSL_LOCAL sw_error_t
    dess_nat_add(a_uint32_t dev_id, fal_nat_entry_t * nat_entry);

    HSL_LOCAL sw_error_t
    dess_nat_get(a_uint32_t dev_id, a_uint32_t get_mode,
                 fal_nat_entry_t * nat_entry);

    HSL_LOCAL sw_error_t
    dess_nat_next(a_uint32_t dev_id, a_uint32_t next_mode,
                  fal_nat_entry_t * nat_entry);

    HSL_LOCAL sw_error_t
    dess_nat_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                          a_uint32_t cnt_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    dess_napt_get(a_uint32_t dev_id, a_uint32_t get_mode,
                  fal_napt_entry_t * napt_entry);

    HSL_LOCAL sw_error_t
    dess_napt_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                           a_uint32_t cnt_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    dess_nat_status_set(a_uint32_t dev_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    dess_nat_status_get(a_uint32_t dev_id, a_bool_t * enable);

    HSL_LOCAL sw_error_t
    dess_nat_hash_mode_set(a_uint32_t dev_id, a_uint32_t mode);

    HSL_LOCAL sw_error_t
    dess_nat_hash_mode_get(a_uint32_t dev_id, a_uint32_t * mode);

    HSL_LOCAL sw_error_t
    dess_napt_status_set(a_uint32_t dev_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    dess_napt_status_get(a_uint32_t dev_id, a_bool_t * enable);

    HSL_LOCAL sw_error_t
    dess_napt_mode_set(a_uint32_t dev_id, fal_napt_mode_t mode);

    HSL_LOCAL sw_error_t
    dess_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode);

    HSL_LOCAL sw_error_t
    dess_nat_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr);

    HSL_LOCAL sw_error_t
    dess_nat_prv_base_mask_set(a_uint32_t dev_id, fal_ip4_addr_t addr);

    HSL_LOCAL sw_error_t
    dess_nat_prv_base_mask_get(a_uint32_t dev_id, fal_ip4_addr_t * addr);

    HSL_LOCAL sw_error_t
    dess_nat_pub_addr_next(a_uint32_t dev_id, a_uint32_t next_mode,
                           fal_nat_pub_addr_t * entry);

    HSL_LOCAL sw_error_t
    dess_nat_unk_session_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);

    HSL_LOCAL sw_error_t
    dess_nat_unk_session_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

    HSL_LOCAL sw_error_t
    dess_nat_global_set(a_uint32_t dev_id, a_bool_t enable);

#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _DESS_NAT_H_ */
