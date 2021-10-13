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
 * @defgroup fal_qm FAL_QM
 * @{
 */
#include "sw.h"
#include "fal_qm.h"
#include "hsl_api.h"
#include "adpt.h"

#ifndef IN_QM_MINI
sw_error_t
_fal_ucast_hash_map_set(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t queue_hash)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ucast_hash_map_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ucast_hash_map_set(dev_id, profile, rss_hash, queue_hash);
	return rv;
}
sw_error_t
_fal_ac_dynamic_threshold_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_dynamic_threshold_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_dynamic_threshold_get(dev_id, queue_id, cfg);
	return rv;
}
sw_error_t
_fal_ucast_queue_base_profile_get(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t *queue_base, a_uint8_t *profile)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ucast_queue_base_profile_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ucast_queue_base_profile_get(dev_id, queue_dest, queue_base, profile);
	return rv;
}
sw_error_t
_fal_port_mcast_priority_class_get(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t *queue_class)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_mcast_priority_class_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_mcast_priority_class_get(dev_id, port, priority, queue_class);
	return rv;
}
#endif
sw_error_t
_fal_ac_dynamic_threshold_set(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_dynamic_threshold_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_dynamic_threshold_set(dev_id, queue_id, cfg);
	return rv;
}
sw_error_t
_fal_ac_prealloc_buffer_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t num)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_prealloc_buffer_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_prealloc_buffer_set(dev_id, obj, num);
	return rv;
}
#ifndef IN_QM_MINI
sw_error_t
_fal_ucast_default_hash_get(
		a_uint32_t dev_id,
		a_uint8_t *hash_value)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ucast_default_hash_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ucast_default_hash_get(dev_id, hash_value);
	return rv;
}
sw_error_t
_fal_ucast_default_hash_set(
		a_uint32_t dev_id,
		a_uint8_t hash_value)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ucast_default_hash_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ucast_default_hash_set(dev_id, hash_value);
	return rv;
}
sw_error_t
_fal_ac_queue_group_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t *group_id)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_queue_group_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_queue_group_get(dev_id, queue_id, group_id);
	return rv;
}
sw_error_t
_fal_ac_ctrl_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_ctrl_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_ctrl_get(dev_id, obj, cfg);
	return rv;
}
sw_error_t
_fal_ac_prealloc_buffer_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t *num)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_prealloc_buffer_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_prealloc_buffer_get(dev_id, obj, num);
	return rv;
}
sw_error_t
_fal_port_mcast_priority_class_set(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t queue_class)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_mcast_priority_class_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_mcast_priority_class_set(dev_id, port, priority, queue_class);
	return rv;
}
sw_error_t
_fal_ucast_hash_map_get(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t *queue_hash)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ucast_hash_map_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ucast_hash_map_get(dev_id, profile, rss_hash, queue_hash);
	return rv;
}
#endif
sw_error_t
_fal_ac_static_threshold_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_static_threshold_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_static_threshold_set(dev_id, obj, cfg);
	return rv;
}
sw_error_t
_fal_ac_queue_group_set(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t group_id)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_queue_group_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_queue_group_set(dev_id, queue_id, group_id);
	return rv;
}
#ifndef IN_QM_MINI
sw_error_t
_fal_ac_group_buffer_get(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_group_buffer_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_group_buffer_get(dev_id, group_id, cfg);
	return rv;
}
sw_error_t
_fal_mcast_cpu_code_class_get(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t *queue_class)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_mcast_cpu_code_class_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_mcast_cpu_code_class_get(dev_id, cpu_code, queue_class);
	return rv;
}
#endif
sw_error_t
_fal_ac_ctrl_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_ctrl_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_ctrl_set(dev_id, obj, cfg);
	return rv;
}
#ifndef IN_QM_MINI
sw_error_t
_fal_ucast_priority_class_get(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t *class)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ucast_priority_class_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ucast_priority_class_get(dev_id, profile, priority, class);
	return rv;
}
#endif
sw_error_t
_fal_queue_flush(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint16_t queue_id)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_queue_flush)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_queue_flush(dev_id, port, queue_id);
	return rv;
}
#ifndef IN_QM_MINI
sw_error_t
_fal_mcast_cpu_code_class_set(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t queue_class)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_mcast_cpu_code_class_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_mcast_cpu_code_class_set(dev_id, cpu_code, queue_class);
	return rv;
}
sw_error_t
_fal_ucast_priority_class_set(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t class)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ucast_priority_class_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ucast_priority_class_set(dev_id, profile, priority, class);
	return rv;
}
sw_error_t
_fal_ac_static_threshold_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_static_threshold_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_static_threshold_get(dev_id, obj, cfg);
	return rv;
}
#endif
sw_error_t
_fal_ucast_queue_base_profile_set(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t queue_base, a_uint8_t profile)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ucast_queue_base_profile_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ucast_queue_base_profile_set(dev_id, queue_dest, queue_base, profile);
	return rv;
}
sw_error_t
_fal_ac_group_buffer_set(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ac_group_buffer_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ac_group_buffer_set(dev_id, group_id, cfg);
	return rv;
}

#ifndef IN_QM_MINI
sw_error_t
_fal_queue_counter_ctrl_set(a_uint32_t dev_id, a_bool_t cnt_en)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_queue_counter_ctrl_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_queue_counter_ctrl_set(dev_id, cnt_en);
	return rv;
}

sw_error_t
_fal_queue_counter_ctrl_get(a_uint32_t dev_id, a_bool_t *cnt_en)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_queue_counter_ctrl_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_queue_counter_ctrl_get(dev_id, cnt_en);
	return rv;
}

sw_error_t
_fal_queue_counter_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_queue_stats_t *info)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_queue_counter_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_queue_counter_get(dev_id, queue_id, info);
	return rv;
}

sw_error_t
_fal_queue_counter_cleanup(a_uint32_t dev_id, a_uint32_t queue_id)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_queue_counter_cleanup)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_queue_counter_cleanup(dev_id, queue_id);
	return rv;
}
#endif

sw_error_t
_fal_qm_enqueue_ctrl_set(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t enable)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_qm_enqueue_ctrl_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_qm_enqueue_ctrl_set(dev_id, queue_id, enable);
	return rv;
}

#ifndef IN_QM_MINI
sw_error_t
_fal_qm_enqueue_ctrl_get(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t *enable)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_qm_enqueue_ctrl_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_qm_enqueue_ctrl_get(dev_id, queue_id, enable);
	return rv;
}

sw_error_t
_fal_qm_port_source_profile_set(a_uint32_t dev_id, fal_port_t port, a_uint32_t src_profile)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_qm_port_source_profile_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_qm_port_source_profile_set(dev_id, port, src_profile);
	return rv;
}
sw_error_t
_fal_qm_port_source_profile_get(a_uint32_t dev_id, fal_port_t port, a_uint32_t *src_profile)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_qm_port_source_profile_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_qm_port_source_profile_get(dev_id, port, src_profile);
	return rv;
}

/*insert flag for inner fal, don't remove it*/

sw_error_t
fal_ucast_hash_map_set(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t queue_hash)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ucast_hash_map_set(dev_id, profile, rss_hash, queue_hash);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ac_dynamic_threshold_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_dynamic_threshold_get(dev_id, queue_id, cfg);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ucast_queue_base_profile_get(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t *queue_base, a_uint8_t *profile)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ucast_queue_base_profile_get(dev_id, queue_dest, queue_base, profile);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_port_mcast_priority_class_get(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t *queue_class)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_port_mcast_priority_class_get(dev_id, port, priority, queue_class);
	FAL_API_UNLOCK;
	return rv;
}
#endif
sw_error_t
fal_ac_dynamic_threshold_set(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_dynamic_threshold_set(dev_id, queue_id, cfg);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ac_prealloc_buffer_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t num)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_prealloc_buffer_set(dev_id, obj, num);
	FAL_API_UNLOCK;
	return rv;
}
#ifndef IN_QM_MINI
sw_error_t
fal_ucast_default_hash_get(
		a_uint32_t dev_id,
		a_uint8_t *hash_value)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ucast_default_hash_get(dev_id, hash_value);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ucast_default_hash_set(
		a_uint32_t dev_id,
		a_uint8_t hash_value)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ucast_default_hash_set(dev_id, hash_value);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ac_queue_group_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t *group_id)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_queue_group_get(dev_id, queue_id, group_id);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ac_ctrl_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_ctrl_get(dev_id, obj, cfg);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ac_prealloc_buffer_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t *num)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_prealloc_buffer_get(dev_id, obj, num);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_port_mcast_priority_class_set(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t queue_class)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_port_mcast_priority_class_set(dev_id, port, priority, queue_class);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ucast_hash_map_get(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t *queue_hash)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ucast_hash_map_get(dev_id, profile, rss_hash, queue_hash);
	FAL_API_UNLOCK;
	return rv;
}
#endif
sw_error_t
fal_ac_static_threshold_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_static_threshold_set(dev_id, obj, cfg);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ac_queue_group_set(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t group_id)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_queue_group_set(dev_id, queue_id, group_id);
	FAL_API_UNLOCK;
	return rv;
}
#ifndef IN_QM_MINI
sw_error_t
fal_ac_group_buffer_get(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_group_buffer_get(dev_id, group_id, cfg);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_mcast_cpu_code_class_get(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t *queue_class)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_mcast_cpu_code_class_get(dev_id, cpu_code, queue_class);
	FAL_API_UNLOCK;
	return rv;
}
#endif
sw_error_t
fal_ac_ctrl_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_ctrl_set(dev_id, obj, cfg);
	FAL_API_UNLOCK;
	return rv;
}
#ifndef IN_QM_MINI
sw_error_t
fal_ucast_priority_class_get(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t *class)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ucast_priority_class_get(dev_id, profile, priority, class);
	FAL_API_UNLOCK;
	return rv;
}
#endif
sw_error_t
fal_queue_flush(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint16_t queue_id)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_queue_flush(dev_id, port, queue_id);
	FAL_API_UNLOCK;
	return rv;
}
#ifndef IN_QM_MINI
sw_error_t
fal_mcast_cpu_code_class_set(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t queue_class)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_mcast_cpu_code_class_set(dev_id, cpu_code, queue_class);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ucast_priority_class_set(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t class)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ucast_priority_class_set(dev_id, profile, priority, class);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ac_static_threshold_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_static_threshold_get(dev_id, obj, cfg);
	FAL_API_UNLOCK;
	return rv;
}
#endif
sw_error_t
fal_ucast_queue_base_profile_set(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t queue_base, a_uint8_t profile)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ucast_queue_base_profile_set(dev_id, queue_dest, queue_base, profile);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ac_group_buffer_set(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ac_group_buffer_set(dev_id, group_id, cfg);
	FAL_API_UNLOCK;
	return rv;
}

#ifndef IN_QM_MINI
sw_error_t
fal_queue_counter_ctrl_set(a_uint32_t dev_id, a_bool_t cnt_en)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_queue_counter_ctrl_set(dev_id, cnt_en);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_queue_counter_ctrl_get(a_uint32_t dev_id, a_bool_t *cnt_en)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_queue_counter_ctrl_get(dev_id, cnt_en);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_queue_counter_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_queue_stats_t *info)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_queue_counter_get(dev_id, queue_id, info);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_queue_counter_cleanup(a_uint32_t dev_id, a_uint32_t queue_id)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_queue_counter_cleanup(dev_id, queue_id);
	FAL_API_UNLOCK;
	return rv;
}
#endif

sw_error_t
fal_qm_enqueue_ctrl_set(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_qm_enqueue_ctrl_set(dev_id, queue_id, enable);
	FAL_API_UNLOCK;
	return rv;
}

#ifndef IN_QM_MINI
sw_error_t
fal_qm_enqueue_ctrl_get(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_qm_enqueue_ctrl_get(dev_id, queue_id, enable);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_qm_port_source_profile_set(a_uint32_t dev_id, fal_port_t port, a_uint32_t src_profile)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_qm_port_source_profile_set(dev_id, port, src_profile);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_qm_port_source_profile_get(a_uint32_t dev_id, fal_port_t port, a_uint32_t *src_profile)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_qm_port_source_profile_get(dev_id, port, src_profile);
	FAL_API_UNLOCK;
	return rv;
}
#endif

EXPORT_SYMBOL(fal_ac_ctrl_set);

EXPORT_SYMBOL(fal_ac_prealloc_buffer_set);

EXPORT_SYMBOL(fal_ac_queue_group_set);

EXPORT_SYMBOL(fal_ac_static_threshold_set);

EXPORT_SYMBOL(fal_ac_dynamic_threshold_set);

EXPORT_SYMBOL(fal_ac_group_buffer_set);

EXPORT_SYMBOL(fal_ucast_queue_base_profile_set);

EXPORT_SYMBOL(fal_queue_flush);

EXPORT_SYMBOL(fal_qm_enqueue_ctrl_set);

#ifndef IN_QM_MINI
EXPORT_SYMBOL(fal_qm_port_source_profile_set);

EXPORT_SYMBOL(fal_qm_port_source_profile_get);

EXPORT_SYMBOL(fal_qm_enqueue_ctrl_get);

EXPORT_SYMBOL(fal_ac_ctrl_get);

EXPORT_SYMBOL(fal_ac_prealloc_buffer_get);

EXPORT_SYMBOL(fal_ac_queue_group_get);

EXPORT_SYMBOL(fal_ac_static_threshold_get);

EXPORT_SYMBOL(fal_ac_dynamic_threshold_get);

EXPORT_SYMBOL(fal_ac_group_buffer_get);

EXPORT_SYMBOL(fal_ucast_queue_base_profile_get);

EXPORT_SYMBOL(fal_ucast_priority_class_set);

EXPORT_SYMBOL(fal_ucast_priority_class_get);

EXPORT_SYMBOL(fal_ucast_hash_map_set);

EXPORT_SYMBOL(fal_ucast_hash_map_get);

EXPORT_SYMBOL(fal_mcast_cpu_code_class_set);

EXPORT_SYMBOL(fal_mcast_cpu_code_class_get);

EXPORT_SYMBOL(fal_port_mcast_priority_class_set);

EXPORT_SYMBOL(fal_port_mcast_priority_class_get);

EXPORT_SYMBOL(fal_queue_counter_ctrl_set);

EXPORT_SYMBOL(fal_queue_counter_ctrl_get);

EXPORT_SYMBOL(fal_queue_counter_get);

EXPORT_SYMBOL(fal_queue_counter_cleanup);
#endif

/*insert flag for outter fal, don't remove it*/
