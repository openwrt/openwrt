/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
#include "fal_vsi.h"
#include "adpt.h"
#include "hsl_api.h"

sw_error_t
_fal_port_vlan_vsi_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t vsi_id)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_vlan_vsi_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_vlan_vsi_set(dev_id, port_id, stag_vid, ctag_vid, vsi_id);
    return rv;
}
sw_error_t
_fal_port_vlan_vsi_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t *vsi_id)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_vlan_vsi_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_vlan_vsi_get(dev_id, port_id, stag_vid, ctag_vid, vsi_id);
    return rv;
}

sw_error_t
_fal_port_vsi_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t vsi_id)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_vsi_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_vsi_set(dev_id, port_id, vsi_id);
    return rv;
}
#ifndef IN_VSI_MINI
sw_error_t
_fal_port_vsi_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *vsi_id)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_vsi_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_vsi_get(dev_id, port_id, vsi_id);
    return rv;
}
#endif
sw_error_t
_fal_vsi_stamove_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_vsi_stamove_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_vsi_stamove_set(dev_id, vsi_id, stamove);
    return rv;
}
#ifndef IN_VSI_MINI
sw_error_t
_fal_vsi_stamove_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_vsi_stamove_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_vsi_stamove_get(dev_id, vsi_id, stamove);
    return rv;
}
sw_error_t
_fal_vsi_newaddr_lrn_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_vsi_newaddr_lrn_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_vsi_newaddr_lrn_get(dev_id, vsi_id, newaddr_lrn);
    return rv;
}
#endif
sw_error_t
_fal_vsi_newaddr_lrn_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_vsi_newaddr_lrn_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_vsi_newaddr_lrn_set(dev_id, vsi_id, newaddr_lrn);
    return rv;
}
sw_error_t
_fal_vsi_member_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_vsi_member_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_vsi_member_set(dev_id, vsi_id, vsi_member);
    return rv;
}
sw_error_t
_fal_vsi_member_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_vsi_member_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_vsi_member_get(dev_id, vsi_id, vsi_member);
    return rv;
}

#ifndef IN_VSI_MINI
sw_error_t
_fal_vsi_counter_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_counter_t *counter)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_vsi_counter_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_vsi_counter_get(dev_id, vsi_id, counter);
    return rv;
}

sw_error_t
_fal_vsi_counter_cleanup(a_uint32_t dev_id, a_uint32_t vsi_id)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_vsi_counter_cleanup)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_vsi_counter_cleanup(dev_id, vsi_id);
    return rv;
}
#endif

/*insert flag for inner fal, don't remove it*/

sw_error_t
fal_port_vlan_vsi_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t vsi_id)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_vlan_vsi_set(dev_id, port_id, stag_vid, ctag_vid, vsi_id);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_vlan_vsi_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t *vsi_id)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_vlan_vsi_get(dev_id, port_id, stag_vid, ctag_vid, vsi_id);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_vsi_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t vsi_id)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_vsi_set(dev_id, port_id, vsi_id);
    FAL_API_UNLOCK;
    return rv;
}
#ifndef IN_VSI_MINI
sw_error_t
fal_port_vsi_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *vsi_id)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_vsi_get(dev_id, port_id, vsi_id);
    FAL_API_UNLOCK;
    return rv;
}
#endif

sw_error_t
fal_vsi_stamove_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_vsi_stamove_set(dev_id, vsi_id, stamove);
    FAL_API_UNLOCK;
    return rv;
}
#ifndef IN_VSI_MINI
sw_error_t
fal_vsi_stamove_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_vsi_stamove_get(dev_id, vsi_id, stamove);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_vsi_newaddr_lrn_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_vsi_newaddr_lrn_get(dev_id, vsi_id, newaddr_lrn);
    FAL_API_UNLOCK;
    return rv;
}
#endif
sw_error_t
fal_vsi_newaddr_lrn_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_vsi_newaddr_lrn_set(dev_id, vsi_id, newaddr_lrn);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_vsi_member_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_vsi_member_set(dev_id, vsi_id, vsi_member);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_vsi_member_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_vsi_member_get(dev_id, vsi_id, vsi_member);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_VSI_MINI
sw_error_t
fal_vsi_counter_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_counter_t *counter)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_vsi_counter_get(dev_id, vsi_id, counter);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_vsi_counter_cleanup(a_uint32_t dev_id, a_uint32_t vsi_id)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_vsi_counter_cleanup(dev_id, vsi_id);
    FAL_API_UNLOCK;
    return rv;
}
#endif
/*insert flag for outter fal, don't remove it*/

#ifndef IN_VSI_MINI
EXPORT_SYMBOL(fal_port_vsi_get);
EXPORT_SYMBOL(fal_vsi_stamove_get);
EXPORT_SYMBOL(fal_vsi_newaddr_lrn_get);
EXPORT_SYMBOL(fal_vsi_counter_get);
EXPORT_SYMBOL(fal_vsi_counter_cleanup);
#endif
EXPORT_SYMBOL(fal_port_vlan_vsi_set);
EXPORT_SYMBOL(fal_port_vlan_vsi_get);
EXPORT_SYMBOL(fal_port_vsi_set);
EXPORT_SYMBOL(fal_vsi_stamove_set);
EXPORT_SYMBOL(fal_vsi_newaddr_lrn_set);
EXPORT_SYMBOL(fal_vsi_member_set);
EXPORT_SYMBOL(fal_vsi_member_get);
