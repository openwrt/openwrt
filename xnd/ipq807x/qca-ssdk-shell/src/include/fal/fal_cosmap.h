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


/**
 * @defgroup fal_cosmap FAL_COSMAP
 * @{
 */
#ifndef _FAL_COSMAP_H_
#define _FAL_COSMAP_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

    typedef struct
    {
        a_bool_t remark_dscp;
        a_bool_t remark_up;
        a_bool_t remark_dei;
        a_uint8_t g_dscp;
        a_uint8_t y_dscp;
        a_uint8_t g_up;
        a_uint8_t y_up;
        a_uint8_t g_dei;
        a_uint8_t y_dei;
    } fal_egress_remark_table_t;

    sw_error_t
    fal_cosmap_dscp_to_pri_set(a_uint32_t dev_id, a_uint32_t dscp,
                               a_uint32_t pri);

    sw_error_t
    fal_cosmap_dscp_to_pri_get(a_uint32_t dev_id, a_uint32_t dscp,
                               a_uint32_t * pri);

    sw_error_t
    fal_cosmap_dscp_to_dp_set(a_uint32_t dev_id, a_uint32_t dscp,
                              a_uint32_t dp);

    sw_error_t
    fal_cosmap_dscp_to_dp_get(a_uint32_t dev_id, a_uint32_t dscp,
                              a_uint32_t * dp);

    sw_error_t
    fal_cosmap_up_to_pri_set(a_uint32_t dev_id, a_uint32_t up,
                             a_uint32_t pri);

    sw_error_t
    fal_cosmap_up_to_pri_get(a_uint32_t dev_id, a_uint32_t up,
                             a_uint32_t * pri);

    sw_error_t
    fal_cosmap_up_to_dp_set(a_uint32_t dev_id, a_uint32_t up,
                            a_uint32_t dp);

    sw_error_t
    fal_cosmap_up_to_dp_get(a_uint32_t dev_id, a_uint32_t up,
                            a_uint32_t * dp);

    sw_error_t
    fal_cosmap_dscp_to_ehpri_set(a_uint32_t dev_id, a_uint32_t dscp,
                               a_uint32_t pri);

    sw_error_t
    fal_cosmap_dscp_to_ehpri_get(a_uint32_t dev_id, a_uint32_t dscp,
                               a_uint32_t * pri);

    sw_error_t
    fal_cosmap_dscp_to_ehdp_set(a_uint32_t dev_id, a_uint32_t dscp,
                              a_uint32_t dp);

    sw_error_t
    fal_cosmap_dscp_to_ehdp_get(a_uint32_t dev_id, a_uint32_t dscp,
                              a_uint32_t * dp);

    sw_error_t
    fal_cosmap_up_to_ehpri_set(a_uint32_t dev_id, a_uint32_t up,
                             a_uint32_t pri);

    sw_error_t
    fal_cosmap_up_to_ehpri_get(a_uint32_t dev_id, a_uint32_t up,
                             a_uint32_t * pri);

    sw_error_t
    fal_cosmap_up_to_ehdp_set(a_uint32_t dev_id, a_uint32_t up,
                            a_uint32_t dp);

    sw_error_t
    fal_cosmap_up_to_ehdp_get(a_uint32_t dev_id, a_uint32_t up,
                            a_uint32_t * dp);

    sw_error_t
    fal_cosmap_pri_to_queue_set(a_uint32_t dev_id, a_uint32_t pri,
                                a_uint32_t queue);

    sw_error_t
    fal_cosmap_pri_to_queue_get(a_uint32_t dev_id, a_uint32_t pri,
                                a_uint32_t * queue);

    sw_error_t
    fal_cosmap_pri_to_ehqueue_set(a_uint32_t dev_id, a_uint32_t pri,
                                  a_uint32_t queue);

    sw_error_t
    fal_cosmap_pri_to_ehqueue_get(a_uint32_t dev_id, a_uint32_t pri,
                                  a_uint32_t * queue);

    sw_error_t
    fal_cosmap_egress_remark_set(a_uint32_t dev_id, a_uint32_t tbl_id,
                                 fal_egress_remark_table_t * tbl);

    sw_error_t
    fal_cosmap_egress_remark_get(a_uint32_t dev_id, a_uint32_t tbl_id,
                                 fal_egress_remark_table_t * tbl);
#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_COSMAP_H_ */

/**
 * @}
 */

