/*
 * Copyright (c) 2012, 2017, The Linux Foundation. All rights reserved.
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



#include "sw_config.h"
#include "aos_head.h"
#include "sw_error.h"
#include "shared_func.h"
#include "fal_type.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "hsl_phy.h"


typedef struct
{
    a_uint32_t phy_id[SW_MAX_NR_PORT];
    fal_pbmp_t dev_portmap;
    fal_pbmp_t property[HSL_PP_BUTT];
} port_info_t;

static port_info_t *p_port_info[SW_MAX_NR_DEV] = { 0 };

a_bool_t
hsl_port_prop_check(a_uint32_t dev_id, fal_port_t port_id,
                    hsl_port_prop_t p_type)
{
    fal_pbmp_t pbitmap;

    if (dev_id >= SW_MAX_NR_DEV)
        return A_FALSE;

    if (HSL_PP_BUTT <= p_type)
    {
        return A_FALSE;
    }

    pbitmap = p_port_info[dev_id]->property[p_type];

    return SW_IS_PBMP_MEMBER(pbitmap, port_id);
}

a_bool_t
hsl_mports_prop_check(a_uint32_t dev_id, fal_pbmp_t port_bitmap,
                      hsl_port_prop_t p_type)
{
    fal_pbmp_t pbitmap;

    if (dev_id >= SW_MAX_NR_DEV)
        return A_FALSE;

    if (HSL_PP_BUTT <= p_type)
    {
        return A_FALSE;
    }

    pbitmap = p_port_info[dev_id]->property[p_type];

    return (SW_IS_PBMP_INCLUDE(pbitmap, port_bitmap));
}

a_bool_t
hsl_port_validity_check(a_uint32_t dev_id, fal_port_t port_id)
{
    fal_pbmp_t pbitmap;

    if (dev_id >= SW_MAX_NR_DEV)
        return A_FALSE;

    pbitmap = p_port_info[dev_id]->dev_portmap;

    return SW_IS_PBMP_MEMBER(pbitmap, port_id);
}

a_bool_t
hsl_mports_validity_check(a_uint32_t dev_id, fal_pbmp_t port_bitmap)
{
    fal_pbmp_t pbitmap;

    if (dev_id >= SW_MAX_NR_DEV)
        return A_FALSE;

    pbitmap = p_port_info[dev_id]->dev_portmap;

    return (SW_IS_PBMP_INCLUDE(pbitmap, port_bitmap));
}

sw_error_t
hsl_port_prop_set(a_uint32_t dev_id, fal_port_t port_id, hsl_port_prop_t p_type)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_validity_check(dev_id, port_id))
    {
        return SW_OUT_OF_RANGE;
    }

    if (HSL_PP_BUTT <= p_type)
    {
        return SW_BAD_PARAM;
    }

    SW_PBMP_ADD_PORT(p_port_info[dev_id]->property[p_type], port_id);

    return SW_OK;
}

sw_error_t
hsl_port_prop_clr(a_uint32_t dev_id, fal_port_t port_id, hsl_port_prop_t p_type)
{
    HSL_DEV_ID_CHECK(dev_id);;

    if (A_FALSE == hsl_port_validity_check(dev_id, port_id))
    {
        return SW_OUT_OF_RANGE;
    }

    if (HSL_PP_BUTT <= p_type)
    {
        return SW_BAD_PARAM;
    }

    SW_PBMP_DEL_PORT(p_port_info[dev_id]->property[p_type], port_id);

    return SW_OK;
}

sw_error_t
hsl_port_prop_get_phyid(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * phy_id)
{
    HSL_DEV_ID_CHECK(dev_id);
	HSL_PORT_ID_CHECK(port_id);

    if (A_FALSE == hsl_port_validity_check(dev_id, port_id))
    {
        return SW_BAD_PARAM;
    }

    *phy_id = qca_ssdk_port_to_phy_addr(dev_id, port_id);

    return SW_OK;
}

sw_error_t
hsl_port_prop_set_phyid(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t phy_id)
{
    HSL_DEV_ID_CHECK(dev_id);
	HSL_PORT_ID_CHECK(port_id);

    if (A_FALSE == hsl_port_validity_check(dev_id, port_id))
    {
        return SW_BAD_PARAM;
    }

    p_port_info[dev_id]->phy_id[port_id] = phy_id;
    return SW_OK;
}

sw_error_t
hsl_port_prop_portmap_set(a_uint32_t dev_id, fal_port_t port_id)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (port_id > SW_MAX_NR_PORT)
        return SW_OUT_OF_RANGE;

    SW_PBMP_ADD_PORT(p_port_info[dev_id]->dev_portmap, port_id);

    return SW_OK;
}

sw_error_t
hsl_port_prop_init_by_dev(a_uint32_t dev_id)
{
    port_info_t *p_mem;

    HSL_DEV_ID_CHECK(dev_id);

    p_mem = aos_mem_alloc(sizeof (port_info_t));
    if (p_mem == NULL)
        return SW_OUT_OF_MEM;

    aos_mem_zero(p_mem, sizeof (port_info_t));
    p_port_info[dev_id] = p_mem;

    return SW_OK;
}

sw_error_t
hsl_port_prop_cleanup_by_dev(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (p_port_info[dev_id] != NULL)
        aos_mem_free((void *)p_port_info[dev_id]);

    p_port_info[dev_id] = NULL;

    return SW_OK;
}


sw_error_t
hsl_port_prop_init(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
    	return SW_BAD_VALUE;

    p_port_info[dev_id] = NULL;

    return SW_OK;
}
