/*
 * Copyright (c) 2014, 2016-2017, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_mirror FAL_MIRROR
 * @{
 */
#ifndef _FAL_MIRROR_H_
#define _FAL_MIRROR_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"


typedef struct
{
	fal_port_t port_id;
	a_uint32_t priority;
} fal_mirr_analysis_config_t;

typedef enum
{
	FAL_MIRR_INGRESS= 0,
	FAL_MIRR_EGRESS,
	FAL_MIRR_BOTH,
} fal_mirr_direction_t;

enum
{
	FUNC_MIRR_ANALYSIS_PORT_SET = 0,
	FUNC_MIRR_ANALYSIS_PORT_GET,
	FUNC_MIRR_PORT_IN_SET,
	FUNC_MIRR_PORT_IN_GET,
	FUNC_MIRR_PORT_EG_SET,
	FUNC_MIRR_PORT_EG_GET,
	FUNC_MIRR_ANALYSIS_CONFIG_SET,
	FUNC_MIRR_ANALYSIS_CONFIG_GET,
};

sw_error_t 
fal_mirr_analysis_port_set(a_uint32_t dev_id, fal_port_t port_id);

sw_error_t 
fal_mirr_analysis_port_get(a_uint32_t dev_id, fal_port_t * port_id);

sw_error_t
fal_mirr_port_in_set(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable);
sw_error_t
fal_mirr_port_in_get(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable);
sw_error_t
fal_mirr_port_eg_set(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable);
sw_error_t
fal_mirr_port_eg_get(a_uint32_t dev_id, fal_port_t port_id,
                  a_bool_t * enable);

sw_error_t
fal_mirr_analysis_config_set(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config);

sw_error_t
fal_mirr_analysis_config_get(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _PORT_MIRROR_H_ */
/**
 * @}
 */
