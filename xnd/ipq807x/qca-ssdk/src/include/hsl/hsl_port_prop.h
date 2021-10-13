/*
 * Copyright (c) 2012, 2017, 2019, The Linux Foundation. All rights reserved.
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



#ifndef _HSL_PORT_PROPERTY_H_
#define _HSL_PORT_PROPERTY_H_

#ifdef __cplusplus
extern "c" {
#endif

    typedef enum {
        HSL_PP_PHY = 0,    /* setting concerning phy */
        HSL_PP_INCL_CPU,   /* setting may include cpu port */
        HSL_PP_EXCL_CPU,   /* setting exclude cpu port */
        HSL_PP_INNER,   /* setting inner ports */
        HSL_PP_CPU, /* setting cpu ports */
        HSL_PP_BUTT
    }
    hsl_port_prop_t;

    a_bool_t
    hsl_port_prop_check(a_uint32_t dev_id, fal_port_t port_id,
                        hsl_port_prop_t p_type);

    a_bool_t
    hsl_mports_prop_check(a_uint32_t dev_id, fal_pbmp_t port_bitmap,
                          hsl_port_prop_t p_type);
    a_bool_t
    hsl_port_validity_check(a_uint32_t dev_id, fal_port_t port_id);

    a_bool_t
    hsl_mports_validity_check(a_uint32_t dev_id, fal_pbmp_t port_bitmap);

    sw_error_t
    hsl_port_prop_portmap_get(a_uint32_t dev_id, fal_pbmp_t port_bitmap);

    sw_error_t
    hsl_port_prop_set(a_uint32_t dev_id, fal_port_t port_id,
                      hsl_port_prop_t p_type);

    sw_error_t
    hsl_port_prop_portmap_set(a_uint32_t dev_id,  fal_port_t port_id);

    sw_error_t
    hsl_port_prop_clr(a_uint32_t dev_id, fal_port_t port_id,
                      hsl_port_prop_t p_type);

    sw_error_t
    hsl_port_prop_get_phyid(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t *phy_id);

    sw_error_t
    hsl_port_prop_set_phyid(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t phy_id);

    sw_error_t
    hsl_port_prop_init_by_dev(a_uint32_t dev_id);

    sw_error_t
    hsl_port_prop_cleanup_by_dev(a_uint32_t dev_id);

    sw_error_t
    hsl_port_prop_init(a_uint32_t dev_id);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /*_HSL_PORT_PROPERTY_H_ */
