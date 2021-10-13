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
 * @defgroup fal_vlan FAL_VLAN
 * @{
 */
#ifndef _FAL_VLAN_H
#define _FAL_VLAN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

    /**
    @brief This structure defines vlan entry.
    */
    typedef struct
    {
        a_uint16_t vid;     /**<  vlan entry id */
        a_uint16_t fid;     /**<   filter data base id*/
        fal_pbmp_t mem_ports;       /**<  member port bit map */
        fal_pbmp_t tagged_ports;    /**<   bit map of tagged infomation for member port*/
        fal_pbmp_t untagged_ports;  /**<   bit map of untagged infomation for member port*/
        fal_pbmp_t unmodify_ports;/**<   bit map of unmodified infomation for member port*/
        fal_pbmp_t u_ports;
        a_bool_t   learn_dis;   /**< disable address learning*/
        a_bool_t   vid_pri_en;  /**<   enable 802.1p*/
        a_uint8_t  vid_pri;     /**<   vlaue of 802.1p when enable vid_pri_en*/
    } fal_vlan_t;


    sw_error_t
    fal_vlan_entry_append(a_uint32_t dev_id, fal_vlan_t * vlan_entry);



    sw_error_t
    fal_vlan_create(a_uint32_t dev_id, a_uint32_t vlan_id);



    sw_error_t
    fal_vlan_next(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan);



    sw_error_t
    fal_vlan_find(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan);



    sw_error_t
    fal_vlan_member_update(a_uint32_t dev_id, a_uint32_t vlan_id,
                           fal_pbmp_t member, fal_pbmp_t u_member);



    sw_error_t
    fal_vlan_delete(a_uint32_t dev_id, a_uint32_t vlan_id);



    sw_error_t
    fal_vlan_reset(a_uint32_t dev_id);


    sw_error_t
    fal_vlan_flush(a_uint32_t dev_id);


    sw_error_t
    fal_vlan_init(a_uint32_t dev_id);


    sw_error_t
    fal_vlan_cleanup(void);


    sw_error_t
    fal_vlan_fid_set(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t fid);


    sw_error_t
    fal_vlan_fid_get(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t * fid);


    sw_error_t
    fal_vlan_member_add(a_uint32_t dev_id, a_uint32_t vlan_id,
                        fal_port_t port_id, fal_pt_1q_egmode_t port_info);


    sw_error_t
    fal_vlan_member_del(a_uint32_t dev_id, a_uint32_t vlan_id, fal_port_t port_id);


    sw_error_t
    fal_vlan_learning_state_set(a_uint32_t dev_id, a_uint32_t vlan_id,
                                a_bool_t enable);


    sw_error_t
    fal_vlan_learning_state_get(a_uint32_t dev_id, a_uint32_t vlan_id,
                                a_bool_t * enable);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FAL_VLAN_H */
/**
 * @}
 */
