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
 * @defgroup fal_stp FAL_VSI
 * @{
 */
#ifndef _FAL_VSI_H_
#define _FAL_VSI_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

#define FAL_VSI_INVALID 0xffff
#define FAL_VLAN_INVALID 0xffff

typedef struct{
	a_uint32_t lrn_en; /*0: disable new address learn, 1: enable new address learn*/
	fal_fwd_cmd_t action;/*0:forward, 1:drop, 2: copy to CPU, 3: redirect to CPU*/
}fal_vsi_newaddr_lrn_t;

typedef struct{
	a_uint32_t stamove_en;/*0:disable station move, 1: enable station move*/
	fal_fwd_cmd_t action;/*0:forward, 1:drop, 2: copy to CPU, 3: redirect to CPU*/
}fal_vsi_stamove_t;

typedef struct{
	a_uint32_t member_ports;/*VSI member ports for known unicast and multicast*/
	a_uint32_t uuc_ports;/*VSI member ports for unknown unicast*/
	a_uint32_t umc_ports;/*VSI member ports for unknown multicast*/
	a_uint32_t bc_ports;/*VSI member ports for broadcast*/
}fal_vsi_member_t;

typedef struct
{
	a_uint32_t rx_packet_counter;
	a_uint64_t rx_byte_counter;
	a_uint32_t tx_packet_counter;
	a_uint64_t tx_byte_counter;
	a_uint32_t fwd_packet_counter;
	a_uint64_t fwd_byte_counter;
	a_uint32_t drop_packet_counter;
	a_uint64_t drop_byte_counter;
}fal_vsi_counter_t;


enum{
	FUNC_PORT_VLAN_VSI_SET,
	FUNC_PORT_VLAN_VSI_GET,
	FUNC_PORT_VSI_SET,
	FUNC_PORT_VSI_GET,
	FUNC_VSI_STAMOVE_SET,
	FUNC_VSI_STAMOVE_GET,
	FUNC_VSI_NEWADDR_LRN_SET,
	FUNC_VSI_NEWADDR_LRN_GET,
	FUNC_VSI_MEMBER_SET,
	FUNC_VSI_MEMBER_GET,
	FUNC_VSI_COUNTER_GET,
	FUNC_VSI_COUNTER_CLEANUP,
};

sw_error_t
fal_vsi_alloc(a_uint32_t dev_id, a_uint32_t *vsi);

sw_error_t
fal_vsi_free(a_uint32_t dev_id, a_uint32_t vsi);

sw_error_t
fal_port_vsi_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t vsi_id);

sw_error_t
fal_port_vsi_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *vsi_id);

sw_error_t
fal_port_vlan_vsi_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t vsi_id);

sw_error_t
fal_port_vlan_vsi_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t *vsi_id);

sw_error_t
fal_vsi_tbl_dump(a_uint32_t dev_id);

sw_error_t
fal_vsi_newaddr_lrn_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn);

sw_error_t
fal_vsi_newaddr_lrn_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn);

sw_error_t
fal_vsi_stamove_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove);

sw_error_t
fal_vsi_stamove_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove);

sw_error_t
fal_vsi_member_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member);

sw_error_t
fal_vsi_member_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member);

sw_error_t
fal_vsi_counter_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_counter_t *counter);

sw_error_t
fal_vsi_counter_cleanup(a_uint32_t dev_id, a_uint32_t vsi_id);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_VSI_H_ */

/**
 * @}
 */

