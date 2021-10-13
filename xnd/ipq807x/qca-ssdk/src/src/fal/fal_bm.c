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
 * @defgroup fal_qm FAL_BM
 * @{
 */
#include "sw.h"
#include "fal_bm.h"
#include "hsl_api.h"
#include "adpt.h"

#ifndef IN_BM_MINI
sw_error_t
_fal_port_bufgroup_map_get(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t *group)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_bufgroup_map_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_bufgroup_map_get(dev_id, port, group);
	return rv;
}
sw_error_t
_fal_bm_port_reserved_buffer_get(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t *prealloc_buff, a_uint16_t *react_buff)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_bm_port_reserved_buffer_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_bm_port_reserved_buffer_get(dev_id, port, prealloc_buff, react_buff);
	return rv;
}
sw_error_t
_fal_bm_bufgroup_buffer_get(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t *buff_num)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_bm_bufgroup_buffer_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_bm_bufgroup_buffer_get(dev_id, group, buff_num);
	return rv;
}
sw_error_t
_fal_bm_port_dynamic_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_bm_port_dynamic_thresh_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_bm_port_dynamic_thresh_get(dev_id, port, cfg);
	return rv;
}
sw_error_t
_fal_port_bm_ctrl_get(a_uint32_t dev_id, fal_port_t port, a_bool_t *enable)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_bm_ctrl_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_bm_ctrl_get(dev_id, port, enable);
	return rv;
}
#endif
sw_error_t
_fal_bm_bufgroup_buffer_set(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t buff_num)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_bm_bufgroup_buffer_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_bm_bufgroup_buffer_set(dev_id, group, buff_num);
	return rv;
}
sw_error_t
_fal_port_bufgroup_map_set(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t group)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_bufgroup_map_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_bufgroup_map_set(dev_id, port, group);
	return rv;
}
#ifndef IN_BM_MINI
sw_error_t
_fal_bm_port_static_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg)
{
	adpt_api_t *p_api;
	hsl_api_t *p_hsl_api;
	sw_error_t rv = SW_OK;

	if((p_api = adpt_api_ptr_get(dev_id)) != NULL) {
		if (NULL == p_api->adpt_bm_port_static_thresh_get)
			return SW_NOT_SUPPORTED;

		rv = p_api->adpt_bm_port_static_thresh_get(dev_id, port, cfg);
		return rv;
	}

	SW_RTN_ON_NULL(p_hsl_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_hsl_api->port_static_thresh_get)
		return SW_NOT_SUPPORTED;

	rv = p_hsl_api->port_static_thresh_get(dev_id, port, cfg);
	return rv;
}
#endif
sw_error_t
_fal_bm_port_reserved_buffer_set(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t prealloc_buff, a_uint16_t react_buff)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_bm_port_reserved_buffer_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_bm_port_reserved_buffer_set(dev_id, port, prealloc_buff, react_buff);
	return rv;
}
sw_error_t
_fal_bm_port_static_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg)
{
	adpt_api_t *p_api;
	hsl_api_t *p_hsl_api;
	sw_error_t rv = SW_OK;

	if((p_api = adpt_api_ptr_get(dev_id)) != NULL) {
		if (NULL == p_api->adpt_bm_port_static_thresh_set)
			return SW_NOT_SUPPORTED;

		rv = p_api->adpt_bm_port_static_thresh_set(dev_id, port, cfg);
		return rv;
	}

	SW_RTN_ON_NULL(p_hsl_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_hsl_api->port_static_thresh_set)
		return SW_NOT_SUPPORTED;

	rv = p_hsl_api->port_static_thresh_set(dev_id, port, cfg);
	return rv;
}
sw_error_t
_fal_bm_port_dynamic_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_bm_port_dynamic_thresh_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_bm_port_dynamic_thresh_set(dev_id, port, cfg);
	return rv;
}
sw_error_t
_fal_port_bm_ctrl_set(a_uint32_t dev_id, fal_port_t port, a_bool_t enable)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_bm_ctrl_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_bm_ctrl_set(dev_id, port, enable);
	return rv;
}

#ifndef IN_BM_MINI
sw_error_t
_fal_bm_port_counter_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_port_counter_t *counter)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_bm_port_counter_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_bm_port_counter_get(dev_id, port, counter);
	return rv;
}
/*insert flag for inner fal, don't remove it*/

sw_error_t
fal_port_bufgroup_map_get(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t *group)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_port_bufgroup_map_get(dev_id, port, group);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_bm_port_reserved_buffer_get(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t *prealloc_buff, a_uint16_t *react_buff)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_bm_port_reserved_buffer_get(dev_id, port, prealloc_buff, react_buff);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_bm_bufgroup_buffer_get(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t *buff_num)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_bm_bufgroup_buffer_get(dev_id, group, buff_num);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_bm_port_dynamic_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_bm_port_dynamic_thresh_get(dev_id, port, cfg);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_port_bm_ctrl_get(a_uint32_t dev_id, fal_port_t port, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_port_bm_ctrl_get(dev_id, port, enable);
	FAL_API_UNLOCK;
	return rv;
}
#endif

sw_error_t
fal_bm_bufgroup_buffer_set(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t buff_num)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_bm_bufgroup_buffer_set(dev_id, group, buff_num);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_port_bufgroup_map_set(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t group)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_port_bufgroup_map_set(dev_id, port, group);
	FAL_API_UNLOCK;
	return rv;
}
#ifndef IN_BM_MINI
sw_error_t
fal_bm_port_static_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_bm_port_static_thresh_get(dev_id, port, cfg);
	FAL_API_UNLOCK;
	return rv;
}
#endif
sw_error_t
fal_bm_port_reserved_buffer_set(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t prealloc_buff, a_uint16_t react_buff)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_bm_port_reserved_buffer_set(dev_id, port, prealloc_buff, react_buff);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_bm_port_static_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_bm_port_static_thresh_set(dev_id, port, cfg);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_bm_port_dynamic_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_bm_port_dynamic_thresh_set(dev_id, port, cfg);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_port_bm_ctrl_set(a_uint32_t dev_id, fal_port_t port, a_bool_t enable)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_port_bm_ctrl_set(dev_id, port, enable);
	FAL_API_UNLOCK;
	return rv;
}

#ifndef IN_BM_MINI
sw_error_t
fal_bm_port_counter_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_port_counter_t *counter)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_bm_port_counter_get(dev_id, port, counter);
	FAL_API_UNLOCK;
	return rv;
}
#endif

EXPORT_SYMBOL(fal_port_bm_ctrl_set);

EXPORT_SYMBOL(fal_port_bufgroup_map_set);

EXPORT_SYMBOL(fal_bm_bufgroup_buffer_set);

EXPORT_SYMBOL(fal_bm_port_reserved_buffer_set);

EXPORT_SYMBOL(fal_bm_port_dynamic_thresh_set);

#ifndef IN_BM_MINI
EXPORT_SYMBOL(fal_port_bm_ctrl_get);

EXPORT_SYMBOL(fal_port_bufgroup_map_get);

EXPORT_SYMBOL(fal_bm_bufgroup_buffer_get);

EXPORT_SYMBOL(fal_bm_port_reserved_buffer_get);

EXPORT_SYMBOL(fal_bm_port_static_thresh_set);

EXPORT_SYMBOL(fal_bm_port_static_thresh_get);

EXPORT_SYMBOL(fal_bm_port_dynamic_thresh_get);

EXPORT_SYMBOL(fal_bm_port_counter_get);
#endif

/*insert flag for outter fal, don't remove it*/
