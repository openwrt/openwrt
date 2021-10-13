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


/**
 * @defgroup fal_ctrlpkt FAL_CTRLPKT
 * @{
 */
#include "sw.h"
#include "fal_ctrlpkt.h"
#include "hsl_api.h"
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>


/**
 * @}
 */
sw_error_t
_fal_mgmtctrl_ethtype_profile_set(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t ethtype)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mgmtctrl_ethtype_profile_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mgmtctrl_ethtype_profile_set(dev_id, profile_id, ethtype);
    return rv;
}

sw_error_t
_fal_mgmtctrl_ethtype_profile_get(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t * ethtype)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mgmtctrl_ethtype_profile_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mgmtctrl_ethtype_profile_get(dev_id, profile_id, ethtype);
    return rv;
}

sw_error_t
_fal_mgmtctrl_rfdb_profile_set(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mgmtctrl_rfdb_profile_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mgmtctrl_rfdb_profile_set(dev_id, profile_id, addr);
    return rv;
}

sw_error_t
_fal_mgmtctrl_rfdb_profile_get(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mgmtctrl_rfdb_profile_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mgmtctrl_rfdb_profile_get(dev_id, profile_id, addr);
    return rv;
}

sw_error_t
_fal_mgmtctrl_ctrlpkt_profile_add(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mgmtctrl_ctrlpkt_profile_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mgmtctrl_ctrlpkt_profile_add(dev_id, ctrlpkt);
    return rv;
}

sw_error_t
_fal_mgmtctrl_ctrlpkt_profile_del(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mgmtctrl_ctrlpkt_profile_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mgmtctrl_ctrlpkt_profile_del(dev_id, ctrlpkt);
    return rv;
}

sw_error_t
_fal_mgmtctrl_ctrlpkt_profile_getfirst(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mgmtctrl_ctrlpkt_profile_getfirst)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mgmtctrl_ctrlpkt_profile_getfirst(dev_id, ctrlpkt);
    return rv;
}

sw_error_t
_fal_mgmtctrl_ctrlpkt_profile_getnext(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mgmtctrl_ctrlpkt_profile_getnext)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mgmtctrl_ctrlpkt_profile_getnext(dev_id, ctrlpkt);
    return rv;
}

sw_error_t
fal_mgmtctrl_ethtype_profile_set(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t ethtype)
{
    sw_error_t rv = SW_OK;

    FAL_CTRLPKT_API_LOCK;
    rv = _fal_mgmtctrl_ethtype_profile_set(dev_id, profile_id, ethtype);
    FAL_CTRLPKT_API_UNLOCK;
    return rv;
}

sw_error_t
fal_mgmtctrl_ethtype_profile_get(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t * ethtype)
{
    sw_error_t rv = SW_OK;

    FAL_CTRLPKT_API_LOCK;
    rv = _fal_mgmtctrl_ethtype_profile_get(dev_id, profile_id, ethtype);
    FAL_CTRLPKT_API_UNLOCK;
    return rv;
}

sw_error_t
fal_mgmtctrl_rfdb_profile_set(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr)
{
    sw_error_t rv = SW_OK;

    FAL_CTRLPKT_API_LOCK;
    rv = _fal_mgmtctrl_rfdb_profile_set(dev_id, profile_id, addr);
    FAL_CTRLPKT_API_UNLOCK;
    return rv;
}

sw_error_t
fal_mgmtctrl_rfdb_profile_get(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr)
{
    sw_error_t rv = SW_OK;

    FAL_CTRLPKT_API_LOCK;
    rv = _fal_mgmtctrl_rfdb_profile_get(dev_id, profile_id, addr);
    FAL_CTRLPKT_API_UNLOCK;
    return rv;
}

sw_error_t
fal_mgmtctrl_ctrlpkt_profile_add(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
    sw_error_t rv = SW_OK;

    FAL_CTRLPKT_API_LOCK;
    rv = _fal_mgmtctrl_ctrlpkt_profile_add(dev_id, ctrlpkt);
    FAL_CTRLPKT_API_UNLOCK;
    return rv;
}

sw_error_t
fal_mgmtctrl_ctrlpkt_profile_del(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
    sw_error_t rv = SW_OK;

    FAL_CTRLPKT_API_LOCK;
    rv = _fal_mgmtctrl_ctrlpkt_profile_del(dev_id, ctrlpkt);
    FAL_CTRLPKT_API_UNLOCK;
    return rv;
}

sw_error_t
fal_mgmtctrl_ctrlpkt_profile_getfirst(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
    sw_error_t rv = SW_OK;

    FAL_CTRLPKT_API_LOCK;
    rv = _fal_mgmtctrl_ctrlpkt_profile_getfirst(dev_id, ctrlpkt);
    FAL_CTRLPKT_API_UNLOCK;
    return rv;
}

sw_error_t
fal_mgmtctrl_ctrlpkt_profile_getnext(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
    sw_error_t rv = SW_OK;

    FAL_CTRLPKT_API_LOCK;
    rv = _fal_mgmtctrl_ctrlpkt_profile_getnext(dev_id, ctrlpkt);
    FAL_CTRLPKT_API_UNLOCK;
    return rv;
}

EXPORT_SYMBOL(fal_mgmtctrl_ethtype_profile_set);
EXPORT_SYMBOL(fal_mgmtctrl_ethtype_profile_get);
EXPORT_SYMBOL(fal_mgmtctrl_rfdb_profile_set);
EXPORT_SYMBOL(fal_mgmtctrl_rfdb_profile_get);
EXPORT_SYMBOL(fal_mgmtctrl_ctrlpkt_profile_add);
EXPORT_SYMBOL(fal_mgmtctrl_ctrlpkt_profile_del);
EXPORT_SYMBOL(fal_mgmtctrl_ctrlpkt_profile_getfirst);
EXPORT_SYMBOL(fal_mgmtctrl_ctrlpkt_profile_getnext);

