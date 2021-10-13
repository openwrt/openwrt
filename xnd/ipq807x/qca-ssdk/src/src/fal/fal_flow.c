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
 * @defgroup fal_flow FAL_FLOW
 * @{
 */
#include "sw.h"
#include "fal_flow.h"
#include "hsl_api.h"
#include "adpt.h"

#ifndef IN_FLOW_MINI
sw_error_t
_fal_flow_host_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode,
		fal_flow_host_entry_t *flow_host_entry)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_host_add)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_host_add(dev_id, add_mode, flow_host_entry);
	return rv;
}
sw_error_t
_fal_flow_entry_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_entry_t *flow_entry)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_entry_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_entry_get(dev_id, get_mode, flow_entry);
	return rv;
}
sw_error_t
_fal_flow_entry_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_entry_t *flow_entry)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_entry_del)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_entry_del(dev_id, del_mode, flow_entry);
	return rv;
}
sw_error_t
_fal_flow_entry_next(
		a_uint32_t dev_id,
		a_uint32_t next_mode,
		fal_flow_entry_t *flow_entry)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_entry_next)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_entry_next(dev_id, next_mode, flow_entry);
	return rv;
}
sw_error_t
_fal_flow_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_status_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_status_get(dev_id, enable);
	return rv;
}
#endif
sw_error_t
_fal_flow_mgmt_set(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *mgmt)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_ctrl_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_ctrl_set(dev_id, type, dir, mgmt);
	return rv;
}
#ifndef IN_FLOW_MINI
sw_error_t
_fal_flow_age_timer_get(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_age_timer_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_age_timer_get(dev_id, age_timer);
	return rv;
}
sw_error_t
_fal_flow_status_set(a_uint32_t dev_id, a_bool_t enable)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_status_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_status_set(dev_id, enable);
	return rv;
}
sw_error_t
_fal_flow_host_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_host_entry_t *flow_host_entry)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_host_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_host_get(dev_id, get_mode, flow_host_entry);
	return rv;
}
sw_error_t
_fal_flow_host_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_host_entry_t *flow_host_entry)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_host_del)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_host_del(dev_id, del_mode, flow_host_entry);
	return rv;
}
#endif
sw_error_t
_fal_flow_mgmt_get(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *mgmt)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_ctrl_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_ctrl_get(dev_id, type, dir, mgmt);
	return rv;
}
#ifndef IN_FLOW_MINI
sw_error_t
_fal_flow_age_timer_set(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_age_timer_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_age_timer_set(dev_id, age_timer);
	return rv;
}
sw_error_t
_fal_flow_entry_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode, /*index or hash*/
		fal_flow_entry_t *flow_entry)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_entry_add)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_entry_add(dev_id, add_mode, flow_entry);
	return rv;
}

sw_error_t
_fal_flow_global_cfg_get(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_global_cfg_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_global_cfg_get(dev_id, cfg);
	return rv;
}

sw_error_t
_fal_flow_global_cfg_set(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_flow_global_cfg_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_flow_global_cfg_set(dev_id, cfg);
	return rv;
}
/*insert flag for inner fal, don't remove it*/

sw_error_t
fal_flow_host_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode,
		fal_flow_host_entry_t *flow_host_entry)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_host_add(dev_id, add_mode, flow_host_entry);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_flow_entry_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_entry_get(dev_id, get_mode, flow_entry);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_flow_entry_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_entry_del(dev_id, del_mode, flow_entry);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_flow_entry_next(
		a_uint32_t dev_id,
		a_uint32_t next_mode,
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_entry_next(dev_id, next_mode, flow_entry);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_flow_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_status_get(dev_id, enable);
	FAL_API_UNLOCK;
	return rv;
}
#endif
sw_error_t
fal_flow_mgmt_set(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *mgmt)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_mgmt_set(dev_id, type, dir, mgmt);
	FAL_API_UNLOCK;
	return rv;
}
#ifndef IN_FLOW_MINI
sw_error_t
fal_flow_age_timer_get(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_age_timer_get(dev_id, age_timer);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_flow_status_set(a_uint32_t dev_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_status_set(dev_id, enable);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_flow_host_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_host_entry_t *flow_host_entry)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_host_get(dev_id, get_mode, flow_host_entry);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_flow_host_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_host_entry_t *flow_host_entry)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_host_del(dev_id, del_mode, flow_host_entry);
	FAL_API_UNLOCK;
	return rv;
}
#endif
sw_error_t
fal_flow_mgmt_get(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *mgmt)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_mgmt_get(dev_id, type, dir, mgmt);
	FAL_API_UNLOCK;
	return rv;
}
#ifndef IN_FLOW_MINI
sw_error_t
fal_flow_age_timer_set(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_age_timer_set(dev_id, age_timer);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_flow_entry_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode, /*index or hash*/
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_entry_add(dev_id, add_mode, flow_entry);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_flow_global_cfg_get(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_global_cfg_get(dev_id, cfg);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_flow_global_cfg_set(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_flow_global_cfg_set(dev_id, cfg);
	FAL_API_UNLOCK;
	return rv;
}
#endif
/*insert flag for outter fal, don't remove it*/
