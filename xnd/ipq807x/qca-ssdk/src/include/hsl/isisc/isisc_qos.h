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


#ifndef _ISISC_QOS_H_
#define _ISISC_QOS_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_qos.h"

    sw_error_t isisc_qos_init(a_uint32_t dev_id);

#ifdef IN_QOS
#define ISISC_QOS_INIT(rv, dev_id) \
    { \
        rv = isisc_qos_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define ISISC_QOS_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    isisc_qos_queue_tx_buf_status_set(a_uint32_t dev_id,
                                     fal_port_t port_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    isisc_qos_queue_tx_buf_status_get(a_uint32_t dev_id,
                                     fal_port_t port_id,
                                     a_bool_t * enable);

    HSL_LOCAL sw_error_t
    isisc_qos_port_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t enable);


    HSL_LOCAL sw_error_t
    isisc_qos_port_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t * enable);

    HSL_LOCAL sw_error_t
    isisc_qos_port_red_en_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable);

    HSL_LOCAL sw_error_t
    isisc_qos_port_red_en_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t* enable);


    HSL_LOCAL sw_error_t
    isisc_qos_queue_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                                 fal_queue_t queue_id,
                                 a_uint32_t * number);


    HSL_LOCAL sw_error_t
    isisc_qos_queue_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                                 fal_queue_t queue_id,
                                 a_uint32_t * number);


    HSL_LOCAL sw_error_t
    isisc_qos_port_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t * number);


    HSL_LOCAL sw_error_t
    isisc_qos_port_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t * number);


    HSL_LOCAL sw_error_t
    isisc_qos_port_rx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t * number);


    HSL_LOCAL sw_error_t
    isisc_qos_port_rx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t * number);


    HSL_LOCAL sw_error_t
    isisc_qos_port_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_qos_mode_t mode, a_bool_t enable);


    HSL_LOCAL sw_error_t
    isisc_qos_port_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_qos_mode_t mode, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    isisc_qos_port_mode_pri_set(a_uint32_t dev_id, fal_port_t port_id,
                               fal_qos_mode_t mode, a_uint32_t pri);


    HSL_LOCAL sw_error_t
    isisc_qos_port_mode_pri_get(a_uint32_t dev_id, fal_port_t port_id,
                               fal_qos_mode_t mode, a_uint32_t * pri);


    HSL_LOCAL sw_error_t
    isisc_qos_port_sch_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
                               fal_sch_mode_t mode, const a_uint32_t weight[]);


    HSL_LOCAL sw_error_t
    isisc_qos_port_sch_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
                               fal_sch_mode_t * mode, a_uint32_t weight[]);

    HSL_LOCAL sw_error_t
    isisc_qos_port_default_spri_set(a_uint32_t dev_id, fal_port_t port_id,
                                   a_uint32_t spri);


    HSL_LOCAL sw_error_t
    isisc_qos_port_default_spri_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_uint32_t * spri);


    HSL_LOCAL sw_error_t
    isisc_qos_port_default_cpri_set(a_uint32_t dev_id, fal_port_t port_id,
                                   a_uint32_t cpri);


    HSL_LOCAL sw_error_t
    isisc_qos_port_default_cpri_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_uint32_t * cpri);

    sw_error_t
    isisc_qos_port_force_spri_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                        a_bool_t enable);

    sw_error_t
    isisc_qos_port_force_spri_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                        a_bool_t* enable);

    sw_error_t
    isisc_qos_port_force_cpri_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                        a_bool_t enable);

    sw_error_t
    isisc_qos_port_force_cpri_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                        a_bool_t* enable);

    HSL_LOCAL sw_error_t
    isisc_qos_queue_remark_table_set(a_uint32_t dev_id, fal_port_t port_id,
                                    fal_queue_t queue_id, a_uint32_t tbl_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    isisc_qos_queue_remark_table_get(a_uint32_t dev_id, fal_port_t port_id,
                                    fal_queue_t queue_id, a_uint32_t * tbl_id, a_bool_t * enable);


#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _ISISC_QOS_H_ */

