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



#include "sw.h"
#include "sw_ioctl.h"
#include "fal_qm.h"
#include "fal_uk_if.h"

sw_error_t
fal_ac_ctrl_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_AC_CTRL_SET, dev_id, obj, cfg);
    return rv;
}

sw_error_t
fal_ac_ctrl_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_AC_CTRL_GET, dev_id, obj, cfg);
    return rv;
}

sw_error_t
fal_ac_prealloc_buffer_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t num)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_AC_PRE_BUFFER_SET, dev_id, obj, num);
    return rv;
}

sw_error_t
fal_ac_prealloc_buffer_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t *num)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_AC_PRE_BUFFER_GET, dev_id, obj, num);
    return rv;
}

sw_error_t
fal_ac_queue_group_set(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t group_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_GROUP_SET, dev_id, queue_id, group_id);
    return rv;
}

sw_error_t
fal_ac_queue_group_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t *group_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_GROUP_GET, dev_id, queue_id, group_id);
    return rv;
}

sw_error_t
fal_ac_static_threshold_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_STATIC_THRESH_SET, dev_id, obj, cfg);
    return rv;
}

sw_error_t
fal_ac_static_threshold_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_STATIC_THRESH_GET, dev_id, obj, cfg);
    return rv;
}

sw_error_t
fal_ac_dynamic_threshold_set(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_DYNAMIC_THRESH_SET, dev_id, queue_id, cfg);
    return rv;
}

sw_error_t
fal_ac_dynamic_threshold_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_DYNAMIC_THRESH_GET, dev_id, queue_id, cfg);
    return rv;
}

sw_error_t
fal_ac_group_buffer_set(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_GOURP_BUFFER_SET, dev_id, group_id, cfg);
    return rv;
}

sw_error_t
fal_ac_group_buffer_get(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_GOURP_BUFFER_GET, dev_id, group_id, cfg);
    return rv;
}

sw_error_t
fal_ucast_queue_base_profile_set(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t queue_base, a_uint8_t profile)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_UCAST_QUEUE_BASE_PROFILE_SET, dev_id, queue_dest, queue_base, profile);
    return rv;
}

sw_error_t
fal_ucast_queue_base_profile_get(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t *queue_base, a_uint8_t *profile)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_UCAST_QUEUE_BASE_PROFILE_GET, dev_id, queue_dest, queue_base, profile);
    return rv;
}

sw_error_t
fal_ucast_priority_class_set(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t class)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_UCAST_PRIORITY_CLASS_SET, dev_id, profile, priority, class);
    return rv;
}

sw_error_t
fal_ucast_priority_class_get(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t *class)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_UCAST_PRIORITY_CLASS_GET, dev_id, profile, priority, class);
    return rv;
}

sw_error_t
fal_ucast_hash_map_set(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t queue_hash)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_UCAST_HASH_MAP_SET, dev_id, profile, rss_hash, queue_hash);
    return rv;
}

sw_error_t
fal_ucast_hash_map_get(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t *queue_hash)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_UCAST_HASH_MAP_GET, dev_id, profile, rss_hash, queue_hash);
    return rv;
}

sw_error_t
fal_mcast_cpu_code_class_set(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t queue_class)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MCAST_CPUCODE_CLASS_SET, dev_id, cpu_code, queue_class);
    return rv;
}

sw_error_t
fal_mcast_cpu_code_class_get(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t *queue_class)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MCAST_CPUCODE_CLASS_GET, dev_id, cpu_code, queue_class);
    return rv;
}

sw_error_t
fal_port_mcast_priority_class_set(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t queue_class)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MCAST_PRIORITY_CLASS_SET, dev_id, port, priority, queue_class);
    return rv;
}

sw_error_t
fal_port_mcast_priority_class_get(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t *queue_class)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MCAST_PRIORITY_CLASS_GET, dev_id, port, priority, queue_class);
    return rv;
}

sw_error_t
fal_queue_flush(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint16_t queue_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_FLUSH, dev_id, port, queue_id);
    return rv;
}

sw_error_t
fal_ucast_default_hash_set(
		a_uint32_t dev_id,
		a_uint8_t hash_value)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_UCAST_DFLT_HASH_MAP_SET, dev_id, hash_value);
    return rv;
}

sw_error_t
fal_ucast_default_hash_get(
		a_uint32_t dev_id,
		a_uint8_t *hash_value)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_UCAST_DFLT_HASH_MAP_GET, dev_id, hash_value);
    return rv;
}

sw_error_t
fal_queue_counter_ctrl_set(a_uint32_t dev_id, a_bool_t cnt_en)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_CNT_CTRL_SET, dev_id, cnt_en);
    return rv;
}

sw_error_t
fal_queue_counter_ctrl_get(a_uint32_t dev_id, a_bool_t *cnt_en)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_CNT_CTRL_GET, dev_id, cnt_en);
    return rv;
}

sw_error_t
fal_queue_counter_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_queue_stats_t *info)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_CNT_GET, dev_id, queue_id, info);
    return rv;
}

sw_error_t
fal_queue_counter_cleanup(a_uint32_t dev_id, a_uint32_t queue_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_CNT_CLEANUP, dev_id, queue_id);
    return rv;
}

sw_error_t
fal_qm_enqueue_ctrl_set(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QM_ENQUEUE_CTRL_SET, dev_id, queue_id, enable);
    return rv;
}

sw_error_t
fal_qm_enqueue_ctrl_get(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QM_ENQUEUE_CTRL_GET, dev_id, queue_id, enable);
    return rv;
}

sw_error_t
fal_qm_port_source_profile_set(
	a_uint32_t dev_id, fal_port_t port,
	a_uint32_t src_profile)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QM_SOURCE_PROFILE_SET, dev_id, port, src_profile);
    return rv;
}

sw_error_t
fal_qm_port_source_profile_get(
	a_uint32_t dev_id, fal_port_t port,
	a_uint32_t *src_profile)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QM_SOURCE_PROFILE_GET, dev_id, port, src_profile);
    return rv;
}

