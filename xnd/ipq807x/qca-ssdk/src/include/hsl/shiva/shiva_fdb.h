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
 * @defgroup shiva_fdb SHIVA_FDB
 * @{
 */
#ifndef _SHIVA_FDB_H_
#define _SHIVA_FDB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "fal/fal_fdb.h"

    sw_error_t
    shiva_fdb_init(a_uint32_t dev_id);

#ifdef IN_FDB
#define SHIVA_FDB_INIT(rv, dev_id) \
    { \
        rv = shiva_fdb_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define SHIVA_FDB_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    shiva_fdb_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry);


    HSL_LOCAL sw_error_t
    shiva_fdb_del_all(a_uint32_t dev_id, a_uint32_t flag);


    HSL_LOCAL sw_error_t
    shiva_fdb_del_by_port(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t flag);


    HSL_LOCAL sw_error_t
    shiva_fdb_del_by_mac(a_uint32_t dev_id,
                         const fal_fdb_entry_t *entry);


    HSL_LOCAL sw_error_t
    shiva_fdb_first(a_uint32_t dev_id, fal_fdb_entry_t * entry);


    HSL_LOCAL sw_error_t
    shiva_fdb_find(a_uint32_t dev_id, fal_fdb_entry_t * entry);


    HSL_LOCAL sw_error_t
    shiva_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t *enable);


    HSL_LOCAL sw_error_t
    shiva_fdb_age_ctrl_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_fdb_age_ctrl_get(a_uint32_t dev_id, a_bool_t *enable);


    HSL_LOCAL sw_error_t
    shiva_fdb_age_time_set(a_uint32_t dev_id, a_uint32_t * time);


    HSL_LOCAL sw_error_t
    shiva_fdb_age_time_get(a_uint32_t dev_id, a_uint32_t * time);


    HSL_LOCAL sw_error_t
    shiva_fdb_iterate(a_uint32_t dev_id, a_uint32_t * iterator, fal_fdb_entry_t * entry);


#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SHIVA_FDB_H_ */
/**
 * @}
 */
