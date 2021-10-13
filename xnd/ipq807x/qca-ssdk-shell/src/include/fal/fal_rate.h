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
 * @defgroup fal_rate FAL_RATE
 * @{
 */
#ifndef _FAL_RATE_H_
#define _FAL_RATE_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

    /**
    @brief This enum defines storm type
    */
    typedef enum {
        FAL_UNICAST_STORM = 0,      /**<  storm caused by unknown unicast packets */
        FAL_MULTICAST_STORM,        /**<  storm caused by unknown multicast packets */
        FAL_BROADCAST_STORM,        /**<  storm caused by broadcast packets */
        FAL_STORM_TYPE_BUTT
    }
    fal_storm_type_t;



    sw_error_t
    fal_rate_queue_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_uint32_t * speed,
                            a_bool_t enable);



    sw_error_t
    fal_rate_queue_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_uint32_t * speed,
                            a_bool_t * enable);



    sw_error_t
    fal_rate_port_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * speed, a_bool_t enable);



    sw_error_t
    fal_rate_port_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * speed, a_bool_t * enable);



    sw_error_t
    fal_rate_port_inrl_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * speed, a_bool_t enable);



    sw_error_t
    fal_rate_port_inrl_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * speed, a_bool_t * enable);



    sw_error_t
    fal_storm_ctrl_frame_set(a_uint32_t dev_id, fal_port_t port_id,
                             fal_storm_type_t frame_type, a_bool_t enable);



    sw_error_t
    fal_storm_ctrl_frame_get(a_uint32_t dev_id, fal_port_t port_id,
                             fal_storm_type_t frame_type,
                             a_bool_t * enable);



    sw_error_t
    fal_storm_ctrl_rate_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * rate);



    sw_error_t
    fal_storm_ctrl_rate_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * rate);


    typedef enum
    {
        FAL_RATE_MI_100US = 0,
        FAL_RATE_MI_1MS,
        FAL_RATE_MI_10MS,
        FAL_RATE_MI_100MS,
    } fal_rate_mt_t;


    typedef struct
    {
        fal_traffic_unit_t meter_unit;
        a_uint32_t         cir;
        a_uint32_t         eir;
        a_uint32_t         cbs;
        a_uint32_t         ebs;
    } fal_egress_shaper_t;


#define FAL_INGRESS_POLICING_TCP_CTRL     0x2
#define FAL_INGRESS_POLICING_MANAGEMENT   0x4
#define FAL_INGRESS_POLICING_BROAD        0x8
#define FAL_INGRESS_POLICING_UNK_UNI      0x10
#define FAL_INGRESS_POLICING_UNK_MUL      0x20
#define FAL_INGRESS_POLICING_UNI          0x40
#define FAL_INGRESS_POLICING_MUL          0x80


    typedef struct
    {
        a_bool_t           c_enable;
        a_bool_t           e_enable;
        a_bool_t           combine_mode;
        fal_traffic_unit_t meter_unit;
        a_bool_t           color_mode;
        a_bool_t           couple_flag;
        a_bool_t           deficit_en;
        a_uint32_t         cir;
        a_uint32_t         eir;
        a_uint32_t         cbs;
        a_uint32_t         ebs;
        a_uint32_t         c_rate_flag;
        a_uint32_t         e_rate_flag;
        fal_rate_mt_t      c_meter_interval;
        fal_rate_mt_t      e_meter_interval;
    } fal_port_policer_t;


    typedef struct
    {
        a_bool_t           counter_mode;
        fal_traffic_unit_t meter_unit;
        fal_rate_mt_t      meter_interval;
        a_bool_t           color_mode;
        a_bool_t           couple_flag;
        a_bool_t           deficit_en;
        a_uint32_t         cir;
        a_uint32_t         eir;
        a_uint32_t         cbs;
        a_uint32_t         ebs;
        a_uint32_t         counter_high;
        a_uint32_t         counter_low;
    } fal_acl_policer_t;


    sw_error_t
    fal_rate_port_policer_set(a_uint32_t dev_id, fal_port_t port_id,
                              fal_port_policer_t * policer);


    sw_error_t
    fal_rate_port_policer_get(a_uint32_t dev_id, fal_port_t port_id,
                              fal_port_policer_t * policer);


    sw_error_t
    fal_rate_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable, fal_egress_shaper_t * shaper);


    sw_error_t
    fal_rate_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable, fal_egress_shaper_t * shaper);


    sw_error_t
    fal_rate_queue_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                              fal_queue_t queue_id, a_bool_t enable,
                              fal_egress_shaper_t * shaper);


    sw_error_t
    fal_rate_queue_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                              fal_queue_t queue_id, a_bool_t * enable,
                              fal_egress_shaper_t * shaper);


    sw_error_t
    fal_rate_acl_policer_set(a_uint32_t dev_id, a_uint32_t policer_id,
                             fal_acl_policer_t * policer);


    sw_error_t
    fal_rate_acl_policer_get(a_uint32_t dev_id, a_uint32_t policer_id,
                             fal_acl_policer_t * policer);

    sw_error_t
    fal_rate_port_add_rate_byte_set(a_uint32_t dev_id, fal_port_t port_id,
                                    a_uint32_t  number);

    sw_error_t
    fal_rate_port_add_rate_byte_get(a_uint32_t dev_id, fal_port_t port_id,
                                    a_uint32_t  *number);

    sw_error_t
    fal_rate_port_gol_flow_en_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t  enable);
    sw_error_t
    fal_rate_port_gol_flow_en_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t*  enable);



#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_RATE_H_ */
/**
 * @}
 */
