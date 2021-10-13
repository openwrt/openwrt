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
 * @defgroup fal_policer FAL_POLICER
 * @{
 */
#ifndef _FAL_POLICER_H_
#define _FAL_POLICER_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

typedef struct
{
	a_bool_t		meter_en;	/* meter enable or disable */
	a_bool_t		couple_en;	/*  two buckets coupling enable or disable*/
	a_uint32_t	color_mode;	/* color aware or color blind */
	a_uint32_t	frame_type;	/* frame type, bit0:unicast;bit1: unkown unicast;bit2:multicast;bit3: unknown multicast; bit4:broadcast */
	a_uint32_t 	meter_mode;
	a_uint32_t	meter_unit; /* 0:byte based; 1:packet based*/
	a_uint32_t	cir;	/* committed information rate */
	a_uint32_t	cbs;	/* committed burst size */
	a_uint32_t	eir; /* excess information rate */
	a_uint32_t	ebs; /* excess burst size */
} fal_policer_config_t;

typedef struct
{
	a_bool_t yellow_priority_en; /* yellow traffic internal priority change enable*/
	a_bool_t yellow_drop_priority_en; /* yellow traffic internal drop priority change enable*/
	a_bool_t yellow_pcp_en; /* yellow traffic pcp change enable*/
	a_bool_t yellow_dei_en; /* yellow traffic dei change enable*/
	a_uint32_t yellow_priority; /* yellow traffic internal priority value*/
	a_uint32_t yellow_drop_priority; /* yellow traffic internal drop priority value*/
	a_uint32_t yellow_pcp; /* yellow traffic pcp value*/
	a_uint32_t yellow_dei; /* yellow traffic dei value*/
	fal_fwd_cmd_t red_action; /* red traffic drop or forward*/
	a_bool_t red_priority_en; /* red traffic internal priority change enable*/
	a_bool_t red_drop_priority_en; /* red traffic internal drop priority change enable*/
	a_bool_t red_pcp_en; /* red traffic pcp change enable*/
	a_bool_t red_dei_en; /* red traffic dei change enable*/
	a_uint32_t red_priority; /* red traffic internal priority value*/
	a_uint32_t red_drop_priority; /* red traffic internal drop priority value*/
	a_uint32_t red_pcp; /* red traffic pcp value*/
	a_uint32_t red_dei;  /* red traffic dei value*/
}fal_policer_action_t;

typedef struct
{
    a_uint32_t green_packet_counter; /*green packet counter */
    a_uint64_t green_byte_counter; /*green byte counter */
    a_uint32_t yellow_packet_counter; /*yellow packet counter */
    a_uint64_t yellow_byte_counter; /*yellow byte counter */
    a_uint32_t red_packet_counter; /*red packet counter */
    a_uint64_t red_byte_counter; /*red byte counter */
} fal_policer_counter_t;

typedef struct
{
    a_uint32_t policer_drop_packet_counter; /*drop packet counter by policer*/
    a_uint64_t policer_drop_byte_counter; /*drop byte counter by policer */
    a_uint32_t policer_forward_packet_counter; /*forward packet counter by policer*/
    a_uint64_t policer_forward_byte_counter; /*forward byte counter by policer*/
    a_uint32_t policer_bypass_packet_counter; /*bypass packet counter by policer*/
    a_uint64_t policer_bypass_byte_counter; /*bypass byte counter by policer */
} fal_policer_global_counter_t;

enum
{
	FUNC_ADPT_ACL_POLICER_COUNTER_GET = 0,
	FUNC_ADPT_PORT_POLICER_COUNTER_GET,
	FUNC_ADPT_PORT_COMPENSATION_BYTE_GET,
	FUNC_ADPT_PORT_POLICER_ENTRY_GET,
	FUNC_ADPT_PORT_POLICER_ENTRY_SET,
	FUNC_ADPT_ACL_POLICER_ENTRY_GET,
	FUNC_ADPT_ACL_POLICER_ENTRY_SET,
	FUNC_ADPT_POLICER_TIME_SLOT_GET,
	FUNC_ADPT_PORT_COMPENSATION_BYTE_SET,
	FUNC_ADPT_POLICER_TIME_SLOT_SET,
	FUNC_ADPT_POLICER_GLOBAL_COUNTER_GET,
};


sw_error_t
fal_port_policer_entry_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *action);

sw_error_t
fal_port_policer_entry_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *atcion);

sw_error_t
fal_acl_policer_entry_set(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action);

sw_error_t
fal_acl_policer_entry_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action);

sw_error_t
fal_port_policer_counter_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_counter_t *counter);

sw_error_t
fal_acl_policer_counter_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_counter_t *counter);

sw_error_t
fal_port_policer_compensation_byte_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t length);

sw_error_t
fal_port_policer_compensation_byte_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *length);

sw_error_t
fal_policer_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot);

sw_error_t
fal_policer_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot);

sw_error_t
fal_policer_global_counter_get(a_uint32_t dev_id,
		fal_policer_global_counter_t *counter);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_POLICER_H_ */
/**
 * @}
 */
