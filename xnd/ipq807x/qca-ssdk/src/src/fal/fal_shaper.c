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
 * @defgroup fal_shaper FAL_SHAPER
 * @{
 */
#include "sw.h"
#include "fal_shaper.h"
#include "hsl_api.h"
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>

sw_error_t
_fal_flow_shaper_set(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_config_t * shaper)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_flow_shaper_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_flow_shaper_set(dev_id, flow_id, shaper);
    return rv;
}
sw_error_t
_fal_queue_shaper_get(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_config_t * shaper)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_queue_shaper_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_queue_shaper_get(dev_id, queue_id, shaper);
    return rv;
}
sw_error_t
_fal_queue_shaper_token_number_set(a_uint32_t dev_id,a_uint32_t queue_id,
		fal_shaper_token_number_t *token_number)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_queue_shaper_token_number_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_queue_shaper_token_number_set(dev_id, queue_id, token_number);
    return rv;
}
#ifndef IN_SHAPER_MINI
sw_error_t
_fal_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_config_t * shaper)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_shaper_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_shaper_get(dev_id, port_id, shaper);
    return rv;
}
sw_error_t
_fal_flow_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_flow_shaper_time_slot_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_flow_shaper_time_slot_get(dev_id, timeslot);
    return rv;
}
sw_error_t
_fal_port_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_shaper_time_slot_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_shaper_time_slot_get(dev_id, timeslot);
    return rv;
}
#endif
sw_error_t
_fal_flow_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_flow_shaper_time_slot_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_flow_shaper_time_slot_set(dev_id, timeslot);
    return rv;
}
sw_error_t
_fal_port_shaper_token_number_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t *token_number)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_shaper_token_number_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_shaper_token_number_set(dev_id, port_id, token_number);
    return rv;
}
#ifndef IN_SHAPER_MINI
sw_error_t
_fal_queue_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_token_number_t *token_number)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_queue_shaper_token_number_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_queue_shaper_token_number_get(dev_id, queue_id, token_number);
    return rv;
}
sw_error_t
_fal_queue_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_queue_shaper_time_slot_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_queue_shaper_time_slot_get(dev_id, timeslot);
    return rv;
}
sw_error_t
_fal_port_shaper_token_number_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t *token_number)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_shaper_token_number_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_shaper_token_number_get(dev_id, port_id, token_number);
    return rv;
}
#endif
sw_error_t
_fal_flow_shaper_token_number_set(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t *token_number)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_flow_shaper_token_number_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_flow_shaper_token_number_set(dev_id, flow_id, token_number);
    return rv;
}
#ifndef IN_SHAPER_MINI
sw_error_t
_fal_flow_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t *token_number)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_flow_shaper_token_number_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_flow_shaper_token_number_get(dev_id, flow_id, token_number);
    return rv;
}
#endif
sw_error_t
_fal_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_config_t * shaper)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_shaper_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_shaper_set(dev_id, port_id, shaper);
    return rv;
}
sw_error_t
_fal_port_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_shaper_time_slot_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_shaper_time_slot_set(dev_id, timeslot);
    return rv;
}
#ifndef IN_SHAPER_MINI
sw_error_t
_fal_flow_shaper_get(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_config_t * shaper)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_flow_shaper_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_flow_shaper_get(dev_id, flow_id, shaper);
    return rv;
}
#endif
sw_error_t
_fal_queue_shaper_set(a_uint32_t dev_id,a_uint32_t queue_id,
		fal_shaper_config_t * shaper)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_queue_shaper_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_queue_shaper_set(dev_id, queue_id, shaper);
    return rv;
}
sw_error_t
_fal_queue_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_queue_shaper_time_slot_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_queue_shaper_time_slot_set(dev_id, timeslot);
    return rv;
}

sw_error_t
_fal_shaper_ipg_preamble_length_set(a_uint32_t dev_id, a_uint32_t ipg_pre_length)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_shaper_ipg_preamble_length_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_shaper_ipg_preamble_length_set(dev_id, ipg_pre_length);
    return rv;
}

#ifndef IN_SHAPER_MINI
sw_error_t
_fal_shaper_ipg_preamble_length_get(a_uint32_t dev_id, a_uint32_t *ipg_pre_length)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_shaper_ipg_preamble_length_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_shaper_ipg_preamble_length_get(dev_id, ipg_pre_length);
    return rv;
}
#endif

/*insert flag for inner fal, don't remove it*/

sw_error_t
fal_flow_shaper_set(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_config_t * shaper)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_flow_shaper_set(dev_id, flow_id, shaper);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_queue_shaper_get(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_config_t * shaper)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_queue_shaper_get(dev_id, queue_id, shaper);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_queue_shaper_token_number_set(a_uint32_t dev_id,a_uint32_t queue_id,
		fal_shaper_token_number_t *token_number)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_queue_shaper_token_number_set(dev_id, queue_id, token_number);
    FAL_API_UNLOCK;
    return rv;
}
#ifndef IN_SHAPER_MINI
sw_error_t
fal_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_config_t * shaper)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_shaper_get(dev_id, port_id, shaper);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_flow_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_flow_shaper_timeslot_get(dev_id, timeslot);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_shaper_timeslot_get(dev_id, timeslot);
    FAL_API_UNLOCK;
    return rv;
}
#endif
sw_error_t
fal_flow_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_flow_shaper_timeslot_set(dev_id, timeslot);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_shaper_token_number_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t *token_number)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_shaper_token_number_set(dev_id, port_id, token_number);
    FAL_API_UNLOCK;
    return rv;
}
#ifndef IN_SHAPER_MINI
sw_error_t
fal_queue_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_token_number_t *token_number)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_queue_shaper_token_number_get(dev_id, queue_id, token_number);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_queue_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_queue_shaper_timeslot_get(dev_id, timeslot);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_shaper_token_number_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t *token_number)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_shaper_token_number_get(dev_id, port_id, token_number);
    FAL_API_UNLOCK;
    return rv;
}
#endif
sw_error_t
fal_flow_shaper_token_number_set(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t *token_number)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_flow_shaper_token_number_set(dev_id, flow_id, token_number);
    FAL_API_UNLOCK;
    return rv;
}
#ifndef IN_SHAPER_MINI
sw_error_t
fal_flow_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t *token_number)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_flow_shaper_token_number_get(dev_id, flow_id, token_number);
    FAL_API_UNLOCK;
    return rv;
}
#endif
sw_error_t
fal_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_config_t * shaper)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_shaper_set(dev_id, port_id, shaper);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_shaper_timeslot_set(dev_id, timeslot);
    FAL_API_UNLOCK;
    return rv;
}
#ifndef IN_SHAPER_MINI
sw_error_t
fal_flow_shaper_get(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_config_t * shaper)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_flow_shaper_get(dev_id, flow_id, shaper);
    FAL_API_UNLOCK;
    return rv;
}
#endif
sw_error_t
fal_queue_shaper_set(a_uint32_t dev_id,a_uint32_t queue_id,
		fal_shaper_config_t * shaper)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_queue_shaper_set(dev_id, queue_id, shaper);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_queue_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_queue_shaper_timeslot_set(dev_id, timeslot);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_shaper_ipg_preamble_length_set(a_uint32_t dev_id, a_uint32_t ipg_pre_length)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_shaper_ipg_preamble_length_set(dev_id, ipg_pre_length);
    FAL_API_UNLOCK;
    return rv;
}
#ifndef IN_SHAPER_MINI
sw_error_t
fal_shaper_ipg_preamble_length_get(a_uint32_t dev_id, a_uint32_t *ipg_pre_length)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_shaper_ipg_preamble_length_get(dev_id, ipg_pre_length);
    FAL_API_UNLOCK;
    return rv;
}
#endif

EXPORT_SYMBOL(fal_flow_shaper_token_number_set);

EXPORT_SYMBOL(fal_queue_shaper_token_number_set);

EXPORT_SYMBOL(fal_port_shaper_token_number_set);

EXPORT_SYMBOL(fal_port_shaper_timeslot_set);

EXPORT_SYMBOL(fal_flow_shaper_timeslot_set);

EXPORT_SYMBOL(fal_queue_shaper_timeslot_set);

EXPORT_SYMBOL(fal_shaper_ipg_preamble_length_set);

EXPORT_SYMBOL(fal_port_shaper_set);

EXPORT_SYMBOL(fal_queue_shaper_get);

EXPORT_SYMBOL(fal_queue_shaper_set);

EXPORT_SYMBOL(fal_flow_shaper_set);

#ifndef IN_SHAPER_MINI

EXPORT_SYMBOL(fal_port_shaper_get);

EXPORT_SYMBOL(fal_flow_shaper_get);

EXPORT_SYMBOL(fal_queue_shaper_token_number_get);

EXPORT_SYMBOL(fal_flow_shaper_token_number_get);

EXPORT_SYMBOL(fal_port_shaper_token_number_get);

EXPORT_SYMBOL(fal_port_shaper_timeslot_get);

EXPORT_SYMBOL(fal_queue_shaper_timeslot_get);

EXPORT_SYMBOL(fal_flow_shaper_timeslot_get);

EXPORT_SYMBOL(fal_shaper_ipg_preamble_length_get);
#endif

/*insert flag for outter fal, don't remove it*/
