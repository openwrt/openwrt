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
 * @defgroup fal_qos FAL_BM
 * @{
 */
#ifndef _FAL_BM_H_
#define _FAL_BM_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

typedef struct
{
	a_uint16_t max_thresh;  /* Static Maximum threshold */
	a_uint16_t resume_off; /*resume offset */
} fal_bm_static_cfg_t;

typedef struct
{
	a_uint8_t weight; /* port weight in the shared group */
	a_uint16_t shared_ceiling;  /* Maximum shared buffers */
	a_uint16_t resume_off; /*resume offset */
	a_uint16_t resume_min_thresh; /* Minumum thresh for resume */
} fal_bm_dynamic_cfg_t;

enum {
	FUNC_PORT_BUFGROUP_MAP_GET = 0,
	FUNC_BM_PORT_RESERVED_BUFFER_GET,
	FUNC_BM_BUFGROUP_BUFFER_GET,
	FUNC_BM_PORT_DYNAMIC_THRESH_GET,
	FUNC_PORT_BM_CTRL_GET,
	FUNC_BM_BUFGROUP_BUFFER_SET,
	FUNC_PORT_BUFGROUP_MAP_SET,
	FUNC_BM_PORT_STATIC_THRESH_GET,
	FUNC_BM_PORT_RESERVED_BUFFER_SET,
	FUNC_BM_PORT_STATIC_THRESH_SET,
	FUNC_BM_PORT_DYNAMIC_THRESH_SET,
	FUNC_PORT_BM_CTRL_SET,
	FUNC_PORT_TDM_CTRL_SET,
	FUNC_PORT_TDM_TICK_CFG_SET,
	FUNC_BM_PORT_COUNTER_GET,
};

typedef struct
{
	a_uint64_t drop_byte_counter; /*drop byte due to overload*/
	a_uint32_t drop_packet_counter; /*drop packet due to overload*/
	a_uint64_t fc_drop_byte_counter; /*drop byte due to fc*/
	a_uint32_t fc_drop_packet_counter; /*drop packet due to fc*/
	a_uint32_t used_counter; /*total used buffer counter for the port*/
	a_uint32_t react_counter; /*react used buffer counter for the port*/
} fal_bm_port_counter_t;

sw_error_t
fal_port_bm_ctrl_set(a_uint32_t dev_id, fal_port_t port, a_bool_t enable);

sw_error_t
fal_port_bm_ctrl_get(a_uint32_t dev_id, fal_port_t port, a_bool_t *enable);

sw_error_t
fal_port_bufgroup_map_set(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t group);

sw_error_t
fal_port_bufgroup_map_get(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t *group);

sw_error_t
fal_bm_bufgroup_buffer_set(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t buff_num);

sw_error_t
fal_bm_bufgroup_buffer_get(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t *buff_num);

sw_error_t
fal_bm_port_reserved_buffer_set(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t prealloc_buff, a_uint16_t react_buff);

sw_error_t
fal_bm_port_reserved_buffer_get(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t *prealloc_buff, a_uint16_t *react_buff);

sw_error_t
fal_bm_port_static_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg);

sw_error_t
fal_bm_port_static_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg);

sw_error_t
fal_bm_port_dynamic_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg);

sw_error_t
fal_bm_port_dynamic_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg);

sw_error_t
fal_bm_port_counter_get(a_uint32_t dev_id, fal_port_t port,
		fal_bm_port_counter_t *counter);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _PORT_BM_H_ */
/**
 * @}
 */

