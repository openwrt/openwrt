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
 * @defgroup shiva_acl SHIVA_ACL
 * @{
 */
#ifndef _SHIVA_ACL_H_
#define _SHIVA_ACL_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_acl.h"

    sw_error_t
    shiva_acl_init(a_uint32_t dev_id);

    sw_error_t
    shiva_acl_reset(a_uint32_t dev_id);

#ifdef IN_ACL
#define SHIVA_ACL_INIT(rv, dev_id) \
    { \
        rv = shiva_acl_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }

#define SHIVA_ACL_RESET(rv, dev_id) \
    { \
        rv = shiva_acl_reset(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define SHIVA_ACL_INIT(rv, dev_id)
#define SHIVA_ACL_RESET(rv, dev_id)
#endif

#ifdef HSL_STANDALONG

    HSL_LOCAL sw_error_t
    shiva_acl_list_creat(a_uint32_t dev_id, a_uint32_t list_id,
                         a_uint32_t list_pri);


    HSL_LOCAL sw_error_t
    shiva_acl_list_destroy(a_uint32_t dev_id, a_uint32_t list_id);


    HSL_LOCAL sw_error_t
    shiva_acl_rule_add(a_uint32_t dev_id, a_uint32_t list_id,
                       a_uint32_t rule_id, a_uint32_t rule_nr,
                       fal_acl_rule_t * rule);


    HSL_LOCAL sw_error_t
    shiva_acl_rule_delete(a_uint32_t dev_id, a_uint32_t list_id,
                          a_uint32_t rule_id, a_uint32_t rule_nr);


    HSL_LOCAL sw_error_t
    shiva_acl_rule_query(a_uint32_t dev_id, a_uint32_t list_id,
                         a_uint32_t rule_id, fal_acl_rule_t * rule);


    HSL_LOCAL sw_error_t
    shiva_acl_list_bind(a_uint32_t dev_id, a_uint32_t list_id,
                        fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                        a_uint32_t obj_idx);


    HSL_LOCAL sw_error_t
    shiva_acl_list_unbind(a_uint32_t dev_id, a_uint32_t list_id,
                          fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                          a_uint32_t obj_idx);


    HSL_LOCAL sw_error_t
    shiva_acl_status_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    shiva_acl_status_get(a_uint32_t dev_id, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    shiva_acl_list_dump(a_uint32_t dev_id);


    HSL_LOCAL sw_error_t
    shiva_acl_rule_dump(a_uint32_t dev_id);

#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _SHIVA_ACL_H_ */
/**
 * @}
 */
