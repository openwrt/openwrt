/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
 * @defgroup
 * @{
 */
#ifndef _ADPT_CPPE_QOS_H_
#define _ADPT_CPPE_QOS_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

sw_error_t
adpt_cppe_qos_port_pri_set(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_pri_precedence_t *pri);
sw_error_t
adpt_cppe_qos_port_pri_get(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_pri_precedence_t *pri);
sw_error_t
adpt_cppe_qos_cosmap_pcp_get(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t pcp,
					fal_qos_cosmap_t *cosmap);
sw_error_t
adpt_cppe_qos_cosmap_pcp_set(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t pcp,
					fal_qos_cosmap_t *cosmap);
sw_error_t
adpt_cppe_qos_cosmap_dscp_get(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t dscp,
					fal_qos_cosmap_t *cosmap);
sw_error_t
adpt_cppe_qos_cosmap_flow_set(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint16_t flow,
					fal_qos_cosmap_t *cosmap);
sw_error_t
adpt_cppe_qos_port_group_set(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_group_t *group);
sw_error_t
adpt_cppe_qos_cosmap_dscp_set(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t dscp,
					fal_qos_cosmap_t *cosmap);
sw_error_t
adpt_cppe_qos_cosmap_flow_get(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint16_t flow,
					fal_qos_cosmap_t *cosmap);
sw_error_t
adpt_cppe_qos_port_group_get(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_group_t *group);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif

