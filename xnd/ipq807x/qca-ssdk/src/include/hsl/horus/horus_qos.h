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
 * @defgroup horus_qos HORUS_QOS
 * @{
 */
#ifndef _HORUS_QOS_H_
#define _HORUS_QOS_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal/fal_qos.h"

    sw_error_t horus_qos_init(a_uint32_t dev_id);

#ifdef IN_QOS
#define HORUS_QOS_INIT(rv, dev_id) \
    { \
        rv = horus_qos_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define HORUS_QOS_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    horus_qos_queue_tx_buf_status_set(a_uint32_t dev_id,
                                      fal_port_t port_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_qos_queue_tx_buf_status_get(a_uint32_t dev_id,
                                      fal_port_t port_id,
                                      a_bool_t * enable);

    HSL_LOCAL sw_error_t
    horus_qos_port_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                     a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_qos_port_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                     a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_qos_queue_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                                  fal_queue_t queue_id,
                                  a_uint32_t * number);


    HSL_LOCAL sw_error_t
    horus_qos_queue_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                                  fal_queue_t queue_id,
                                  a_uint32_t * number);


    HSL_LOCAL sw_error_t
    horus_qos_port_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t * number);


    HSL_LOCAL sw_error_t
    horus_qos_port_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t * number);


    HSL_LOCAL sw_error_t
    horus_qos_port_rx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t * number);


    HSL_LOCAL sw_error_t
    horus_qos_port_rx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t * number);


    HSL_LOCAL sw_error_t
    horus_cosmap_up_queue_set(a_uint32_t dev_id, a_uint32_t up,
                              fal_queue_t queue);


    HSL_LOCAL sw_error_t
    horus_cosmap_up_queue_get(a_uint32_t dev_id, a_uint32_t up,
                              fal_queue_t * queue);


    HSL_LOCAL sw_error_t
    horus_cosmap_dscp_queue_set(a_uint32_t dev_id, a_uint32_t dscp,
                                fal_queue_t queue);


    HSL_LOCAL sw_error_t
    horus_cosmap_dscp_queue_get(a_uint32_t dev_id, a_uint32_t dscp,
                                fal_queue_t * queue);


    HSL_LOCAL sw_error_t
    horus_qos_port_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_qos_mode_t mode, a_bool_t enable);


    HSL_LOCAL sw_error_t
    horus_qos_port_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_qos_mode_t mode, a_bool_t * enable);


    HSL_LOCAL sw_error_t
    horus_qos_port_mode_pri_set(a_uint32_t dev_id, fal_port_t port_id,
                                fal_qos_mode_t mode, a_uint32_t pri);


    HSL_LOCAL sw_error_t
    horus_qos_port_mode_pri_get(a_uint32_t dev_id, fal_port_t port_id,
                                fal_qos_mode_t mode, a_uint32_t * pri);


    HSL_LOCAL sw_error_t
    horus_qos_port_default_up_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t up);


    HSL_LOCAL sw_error_t
    horus_qos_port_default_up_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t * up);


    HSL_LOCAL sw_error_t
    horus_qos_port_sch_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
                                fal_sch_mode_t mode, const a_uint32_t weight[]);


    HSL_LOCAL sw_error_t
    horus_qos_port_sch_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
                                fal_sch_mode_t * mode, a_uint32_t weight[]);


#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _HORUS_QOS_H_ */
/**
 * @}
 */
