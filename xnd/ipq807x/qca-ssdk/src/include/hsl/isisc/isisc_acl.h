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
 * @defgroup isisc_acl ISISC_ACL
 * @{
 */
#ifndef _ISISC_ACL_H_
#define _ISISC_ACL_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_acl.h"

    sw_error_t isisc_acl_init(a_uint32_t dev_id);

    sw_error_t isisc_acl_reset(a_uint32_t dev_id);

    sw_error_t isisc_acl_cleanup(a_uint32_t dev_id);


#ifdef IN_ACL
#define ISISC_ACL_INIT(rv, dev_id) \
    { \
        rv = isisc_acl_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }

#define ISISC_ACL_RESET(rv, dev_id) \
    { \
        rv = isisc_acl_reset(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#define ISISC_ACL_CLEANUP(rv, dev_id) \
    { \
        rv = isisc_acl_cleanup(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define ISISC_ACL_INIT(rv, dev_id)
#define ISISC_ACL_RESET(rv, dev_id)
#define ISISC_ACL_CLEANUP(rv, dev_id)
#endif

    sw_error_t
    isisc_acl_list_creat(a_uint32_t dev_id, a_uint32_t list_id,
                        a_uint32_t list_pri);

    sw_error_t
    isisc_acl_rule_add(a_uint32_t dev_id, a_uint32_t list_id,
                      a_uint32_t rule_id, a_uint32_t rule_nr,
                      fal_acl_rule_t * rule);

    sw_error_t
    isisc_acl_rule_delete(a_uint32_t dev_id, a_uint32_t list_id,
                         a_uint32_t rule_id, a_uint32_t rule_nr);

    sw_error_t
    isisc_acl_rule_query(a_uint32_t dev_id, a_uint32_t list_id,
                        a_uint32_t rule_id, fal_acl_rule_t * rule);

    a_uint32_t
    isisc_acl_rule_get_offset(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id);

    sw_error_t
    isisc_acl_rule_sync_multi_portmap(a_uint32_t dev_id, a_uint32_t pos, a_uint32_t *act);

    sw_error_t
    isisc_acl_list_bind(a_uint32_t dev_id, a_uint32_t list_id,
                       fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                       a_uint32_t obj_idx);

    sw_error_t
    isisc_acl_list_unbind(a_uint32_t dev_id, a_uint32_t list_id,
                         fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                         a_uint32_t obj_idx);

    sw_error_t
    isisc_acl_status_set(a_uint32_t dev_id, a_bool_t enable);

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    isisc_acl_list_destroy(a_uint32_t dev_id, a_uint32_t list_id);

    HSL_LOCAL sw_error_t
    isisc_acl_status_get(a_uint32_t dev_id, a_bool_t * enable);

    HSL_LOCAL sw_error_t
    isisc_acl_list_dump(a_uint32_t dev_id);

    HSL_LOCAL sw_error_t
    isisc_acl_rule_dump(a_uint32_t dev_id);

    HSL_LOCAL sw_error_t
    isisc_acl_port_udf_profile_set(a_uint32_t dev_id, fal_port_t port_id,
                                  fal_acl_udf_type_t udf_type,
                                  a_uint32_t offset, a_uint32_t length);

    HSL_LOCAL sw_error_t
    isisc_acl_port_udf_profile_get(a_uint32_t dev_id, fal_port_t port_id,
                                  fal_acl_udf_type_t udf_type,
                                  a_uint32_t * offset, a_uint32_t * length);

    HSL_LOCAL sw_error_t
    isisc_acl_rule_active(a_uint32_t dev_id, a_uint32_t list_id,
                         a_uint32_t rule_id, a_uint32_t rule_nr);

    HSL_LOCAL sw_error_t
    isisc_acl_rule_deactive(a_uint32_t dev_id, a_uint32_t list_id,
                           a_uint32_t rule_id, a_uint32_t rule_nr);
    HSL_LOCAL sw_error_t
    isisc_acl_rule_src_filter_sts_set(a_uint32_t dev_id,
                                     a_uint32_t rule_id, a_bool_t enable);

    HSL_LOCAL sw_error_t
    isisc_acl_rule_src_filter_sts_get(a_uint32_t dev_id,
                                     a_uint32_t rule_id, a_bool_t* enable);

#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _ISISC_ACL_H_ */
/**
 * @}
 */
