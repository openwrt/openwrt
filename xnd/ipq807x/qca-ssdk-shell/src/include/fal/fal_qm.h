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
 * @defgroup fal_qos FAL_QM
 * @{
 */
#ifndef _FAL_QM_H_
#define _FAL_QM_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

typedef enum {
	FAL_AC_QUEUE = 0,
	FAL_AC_GROUP
} fal_ac_type_t;

typedef struct {
	fal_ac_type_t type;
	a_uint32_t obj_id;
} fal_ac_obj_t;

typedef struct {
	a_bool_t ac_en; /* 0 for disable and 1 for enable*/
	a_bool_t ac_fc_en; /* ac for flow control packets */
} fal_ac_ctrl_t;

typedef struct {
	a_uint16_t prealloc_buffer; /* guareented buffer number */
	a_uint16_t total_buffer; /* total buffer number */
} fal_ac_group_buffer_t;

typedef struct {
	a_bool_t color_enable; /* 1 for color aware and 0 for color-blind */
	a_bool_t wred_enable; /* 1 for wred and 0 for tail drop */
	a_uint16_t green_max;
	a_uint16_t green_min_off; /*gap between green max and green min*/
	a_uint16_t yel_max_off; /*gap between green max and yel max*/
	a_uint16_t yel_min_off; /*gap between green max and yel min*/
	a_uint16_t red_max_off; /*gap between green max and red max*/
	a_uint16_t red_min_off; /*gap between green max and red min*/
	a_uint16_t green_resume_off; /* green resume offset */
	a_uint16_t yel_resume_off; /* yellow resume offset */
	a_uint16_t red_resume_off; /* red resume offset */
} fal_ac_static_threshold_t;

typedef struct {
	a_bool_t color_enable; /* 1 for color aware and 0 for color-blind */
	a_bool_t wred_enable; /* 1 for wred and 0 for tail drop */
	a_uint8_t shared_weight; /* weight in the shared group */
	a_uint16_t green_min_off; /*gap between green max and green min*/
	a_uint16_t yel_max_off; /*gap between green max and yel max*/
	a_uint16_t yel_min_off; /*gap between green max and yel min*/
	a_uint16_t red_max_off; /*gap between green max and red max*/
	a_uint16_t red_min_off; /*gap between green max and red min*/
	a_uint16_t green_resume_off; /* green resume offset */
	a_uint16_t yel_resume_off; /* yellow resume offset */
	a_uint16_t red_resume_off; /* red resume offset */
	a_uint16_t ceiling; /*shared ceiling*/
} fal_ac_dynamic_threshold_t;

typedef struct {
	a_uint8_t src_profile; /* queue source profile */
	a_bool_t service_code_en;
	a_uint16_t service_code;
	a_bool_t cpu_code_en;
	a_uint16_t cpu_code;
	fal_port_t dst_port; /* destination physical or VP port */
} fal_ucast_queue_dest_t;

#define FAL_QM_DROP_ITEMS	6
typedef struct {
	a_uint32_t tx_packets;
	a_uint64_t tx_bytes;
	a_uint32_t pending_buff_num;
	a_uint32_t drop_packets[FAL_QM_DROP_ITEMS];
	a_uint64_t drop_bytes[FAL_QM_DROP_ITEMS];
} fal_queue_stats_t;

enum {
	FUNC_UCAST_HASH_MAP_SET = 0,
	FUNC_AC_DYNAMIC_THRESHOLD_GET,
	FUNC_UCAST_QUEUE_BASE_PROFILE_GET,
	FUNC_PORT_MCAST_PRIORITY_CLASS_GET,
	FUNC_AC_DYNAMIC_THRESHOLD_SET,
	FUNC_AC_PREALLOC_BUFFER_SET,
	FUNC_UCAST_DEFAULT_HASH_GET,
	FUNC_UCAST_DEFAULT_HASH_SET,
	FUNC_AC_QUEUE_GROUP_GET,
	FUNC_AC_CTRL_GET,
	FUNC_AC_PREALLOC_BUFFER_GET,
	FUNC_PORT_MCAST_PRIORITY_CLASS_SET,
	FUNC_UCAST_HASH_MAP_GET,
	FUNC_AC_STATIC_THRESHOLD_SET,
	FUNC_AC_QUEUE_GROUP_SET,
	FUNC_AC_GROUP_BUFFER_GET,
	FUNC_MCAST_CPU_CODE_CLASS_GET,
	FUNC_AC_CTRL_SET,
	FUNC_UCAST_PRIORITY_CLASS_GET,
	FUNC_QUEUE_FLUSH,
	FUNC_MCAST_CPU_CODE_CLASS_SET,
	FUNC_UCAST_PRIORITY_CLASS_SET,
	FUNC_AC_STATIC_THRESHOLD_GET,
	FUNC_UCAST_QUEUE_BASE_PROFILE_SET,
	FUNC_AC_GROUP_BUFFER_SET,
	FUNC_QUEUE_COUNTER_CLEANUP,
	FUNC_QUEUE_COUNTER_GET,
	FUNC_QUEUE_COUNTER_CTRL_GET,
	FUNC_QUEUE_COUNTER_CTRL_SET,
	FUNC_QM_ENQUEUE_CTRL_GET,
	FUNC_QM_ENQUEUE_CTRL_SET,
	FUNC_QM_SOURCE_PROFILE_GET,
	FUNC_QM_SOURCE_PROFILE_SET,
};

sw_error_t
fal_ac_ctrl_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg);

sw_error_t
fal_ac_ctrl_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg);

sw_error_t
fal_ac_prealloc_buffer_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t num);

sw_error_t
fal_ac_prealloc_buffer_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t *num);

sw_error_t
fal_ac_queue_group_set(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t group_id);

sw_error_t
fal_ac_queue_group_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t *group_id);

sw_error_t
fal_ac_static_threshold_set(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg);

sw_error_t
fal_ac_static_threshold_get(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg);

sw_error_t
fal_ac_dynamic_threshold_set(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg);

sw_error_t
fal_ac_dynamic_threshold_get(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg);

sw_error_t
fal_ac_group_buffer_set(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg);

sw_error_t
fal_ac_group_buffer_get(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg);

sw_error_t
fal_ucast_queue_base_profile_set(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t queue_base, a_uint8_t profile);

sw_error_t
fal_ucast_queue_base_profile_get(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t *queue_base, a_uint8_t *profile);

sw_error_t
fal_ucast_priority_class_set(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t class);

sw_error_t
fal_ucast_priority_class_get(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t *class);

sw_error_t
fal_ucast_hash_map_set(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t queue_hash);

sw_error_t
fal_ucast_hash_map_get(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t *queue_hash);

sw_error_t
fal_mcast_cpu_code_class_set(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t queue_class);

sw_error_t
fal_mcast_cpu_code_class_get(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t *queue_class);

sw_error_t
fal_port_mcast_priority_class_set(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t queue_class);

sw_error_t
fal_port_mcast_priority_class_get(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t *queue_class);

sw_error_t
fal_queue_flush(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint16_t queue_id);

sw_error_t
fal_ucast_default_hash_set(
		a_uint32_t dev_id,
		a_uint8_t hash_value);

sw_error_t
fal_ucast_default_hash_get(
		a_uint32_t dev_id,
		a_uint8_t *hash_value);

sw_error_t
fal_queue_counter_ctrl_set(a_uint32_t dev_id, a_bool_t cnt_en);

sw_error_t
fal_queue_counter_ctrl_get(a_uint32_t dev_id, a_bool_t *cnt_en);

sw_error_t
fal_queue_counter_get(a_uint32_t dev_id, a_uint32_t queue_id, fal_queue_stats_t *info);

sw_error_t
fal_queue_counter_cleanup(a_uint32_t dev_id, a_uint32_t queue_id);

sw_error_t
fal_qm_enqueue_ctrl_set(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t enable);

sw_error_t
fal_qm_enqueue_ctrl_get(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t *enable);

sw_error_t
fal_qm_port_source_profile_set(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint32_t src_profile);

sw_error_t
fal_qm_port_source_profile_get(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint32_t *src_profile);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _PORT_QM_H_ */
/**
 * @}
 */

