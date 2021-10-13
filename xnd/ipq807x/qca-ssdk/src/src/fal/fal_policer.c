/*
 * Copyright (c) 2016-2017, 2021, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_policer FAL_POLICER
 * @{
 */
#include "sw.h"
#include "fal_policer.h"
#include "hsl_api.h"
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>


#ifndef IN_POLICER_MINI
sw_error_t
_fal_acl_policer_counter_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_counter_t *counter)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_acl_policer_counter_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_acl_policer_counter_get(dev_id, index, counter);
    return rv;
}
sw_error_t
_fal_port_policer_counter_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_counter_t *counter)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_policer_counter_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_policer_counter_get(dev_id, port_id, counter);
    return rv;
}
sw_error_t
_fal_port_policer_entry_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_policer_entry_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_policer_entry_get(dev_id, port_id, policer, action);
    return rv;
}
sw_error_t
_fal_port_policer_entry_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_policer_entry_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_policer_entry_set(dev_id, port_id, policer, action);
    return rv;
}
sw_error_t
_fal_acl_policer_entry_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_acl_policer_entry_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_acl_policer_entry_get(dev_id, index, policer, action);
    return rv;
}
sw_error_t
_fal_acl_policer_entry_set(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_acl_policer_entry_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_acl_policer_entry_set(dev_id, index, policer, action);
    return rv;
}
sw_error_t
_fal_policer_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_policer_time_slot_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_policer_time_slot_get(dev_id, timeslot);
    return rv;
}

sw_error_t
_fal_policer_bypass_en_get(a_uint32_t dev_id, fal_policer_frame_type_t frame_type,
	a_bool_t *enable)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_policer_bypass_en_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_policer_bypass_en_get(dev_id, frame_type, enable);
	return rv;
}

#endif
sw_error_t
_fal_policer_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_policer_time_slot_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_policer_time_slot_set(dev_id, timeslot);
    return rv;
}

sw_error_t
_fal_policer_bypass_en_set(a_uint32_t dev_id, fal_policer_frame_type_t frame_type,
	a_bool_t enable)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_policer_bypass_en_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_policer_bypass_en_set(dev_id, frame_type, enable);
	return rv;
}

#ifndef IN_POLICER_MINI
sw_error_t
_fal_port_policer_compensation_byte_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t *length)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_compensation_byte_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_compensation_byte_get(dev_id, port_id, length);
    return rv;
}
#endif
sw_error_t
_fal_port_policer_compensation_byte_set(a_uint32_t dev_id, a_uint32_t port_id,
			a_uint32_t length)

{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_compensation_byte_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_compensation_byte_set(dev_id, port_id, length);
    return rv;
}

#ifndef IN_POLICER_MINI
sw_error_t
_fal_policer_global_counter_get(a_uint32_t dev_id,
		fal_policer_global_counter_t *counter)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_policer_global_counter_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_policer_global_counter_get(dev_id, counter);
	return rv;
}

/*insert flag for inner fal, don't remove it*/

sw_error_t
fal_acl_policer_counter_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_counter_t *counter)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_acl_policer_counter_get(dev_id, index, counter);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_policer_counter_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_counter_t *counter)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_policer_counter_get(dev_id, port_id, counter);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_policer_entry_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_policer_entry_get(dev_id, port_id, policer, action);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_policer_entry_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_policer_entry_set(dev_id, port_id, policer, action);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_acl_policer_entry_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_acl_policer_entry_get(dev_id, index, policer, action);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_acl_policer_entry_set(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_acl_policer_entry_set(dev_id, index, policer, action);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_policer_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_policer_timeslot_get(dev_id, timeslot);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_policer_bypass_en_get(a_uint32_t dev_id, fal_policer_frame_type_t frame_type,
	a_bool_t *enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_policer_bypass_en_get(dev_id, frame_type, enable);
    FAL_API_UNLOCK;
    return rv;
}
#endif
sw_error_t
fal_policer_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_policer_timeslot_set(dev_id, timeslot);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_policer_bypass_en_set(a_uint32_t dev_id, fal_policer_frame_type_t frame_type,
	a_bool_t enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_policer_bypass_en_set(dev_id, frame_type, enable);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_POLICER_MINI
sw_error_t
fal_port_policer_compensation_byte_get(a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t *length)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_policer_compensation_byte_get(dev_id, port_id, length);
    FAL_API_UNLOCK;
    return rv;
}
#endif
sw_error_t
fal_port_policer_compensation_byte_set(a_uint32_t dev_id, fal_port_t port_id,
			a_uint32_t length)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_policer_compensation_byte_set(dev_id, port_id, length);
    FAL_API_UNLOCK;
    return rv;
}
#ifndef IN_POLICER_MINI
sw_error_t
fal_policer_global_counter_get(a_uint32_t dev_id,
		fal_policer_global_counter_t *counter)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_policer_global_counter_get(dev_id, counter);
    FAL_API_UNLOCK;
    return rv;
}
#endif

#ifndef IN_POLICER_MINI
EXPORT_SYMBOL(fal_acl_policer_counter_get);
EXPORT_SYMBOL(fal_port_policer_counter_get);
EXPORT_SYMBOL(fal_port_policer_entry_get);
EXPORT_SYMBOL(fal_port_policer_entry_set);
EXPORT_SYMBOL(fal_acl_policer_entry_get);
EXPORT_SYMBOL(fal_acl_policer_entry_set);
EXPORT_SYMBOL(fal_policer_timeslot_get);
EXPORT_SYMBOL(fal_port_policer_compensation_byte_get);
EXPORT_SYMBOL(fal_policer_global_counter_get);
EXPORT_SYMBOL(fal_policer_bypass_en_get);
#endif
EXPORT_SYMBOL(fal_policer_timeslot_set);
EXPORT_SYMBOL(fal_port_policer_compensation_byte_set);
EXPORT_SYMBOL(fal_policer_bypass_en_set);

/*insert flag for outter fal, don't remove it*/
