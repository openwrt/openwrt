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
 * @defgroup fal_shaper FAL_SHAPER
 * @{
 */
#ifndef _FAL_SHAPER_H_
#define _FAL_SHAPER_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

typedef enum
{
	FAL_IPG_PREAMBLE_FRAME_CRC = 0,  /* IPG + Preamble + Frame + CRC */
	FAL_FRAME_CRC,  /* Frame + CRC */
	FAL_L3_EXCLUDE_CRC  /* after Ethernet type excude CRC*/
} fal_shaper_frame_mode_t;

typedef struct
{
	a_bool_t  couple_en; /* two buckets coupled enable or disable*/
	a_uint32_t meter_unit; /* 0 byte based, 1 frame based */
	a_bool_t	c_shaper_en; /* egress shaer C bucket enable or disable */
	a_uint32_t cbs; /* committed burst size */
	a_uint32_t cir; /* committed information rate */
	a_bool_t	e_shaper_en; /* egress shaper E bucket enable or disable */
	a_uint32_t ebs; /* excess burst size */
	a_uint32_t eir; /* excess information rate */
	fal_shaper_frame_mode_t shaper_frame_mode; /* shaper frame mode */
} fal_shaper_config_t;

typedef struct
{
     a_bool_t      c_token_number_negative_en; /* C token is negative or not */
     a_uint32_t    c_token_number; /* C token value */
     a_bool_t      e_token_number_negative_en; /* E token is negative or not */
     a_uint32_t    e_token_number; /* E token value */
} fal_shaper_token_number_t;

enum
{
	FUNC_ADPT_FLOW_SHAPER_SET = 0,
	FUNC_ADPT_QUEUE_SHAPER_GET,
	FUNC_ADPT_QUEUE_SHAPER_TOKEN_NUMBER_SET,
	FUNC_ADPT_PORT_SHAPER_GET,
	FUNC_ADPT_FLOW_SHAPER_TIME_SLOT_GET,
	FUNC_ADPT_PORT_SHAPER_TIME_SLOT_GET,
	FUNC_ADPT_FLOW_SHAPER_TIME_SLOT_SET,
	FUNC_ADPT_PORT_SHAPER_TOKEN_NUMBER_SET,
	FUNC_ADPT_QUEUE_SHAPER_TOKEN_NUMBER_GET,
	FUNC_ADPT_QUEUE_SHAPER_TIME_SLOT_GET,
	FUNC_ADPT_PORT_SHAPER_TOKEN_NUMBER_GET,
	FUNC_ADPT_FLOW_SHAPER_TOKEN_NUMBER_SET,
	FUNC_ADPT_FLOW_SHAPER_TOKEN_NUMBER_GET,
	FUNC_ADPT_PORT_SHAPER_SET,
	FUNC_ADPT_PORT_SHAPER_TIME_SLOT_SET,
	FUNC_ADPT_FLOW_SHAPER_GET,
	FUNC_ADPT_QUEUE_SHAPER_SET,
	FUNC_ADPT_QUEUE_SHAPER_TIME_SLOT_SET,
	FUNC_ADPT_SHAPER_IPG_PREAMBLE_LENGTH_SET,
	FUNC_ADPT_SHAPER_IPG_PREAMBLE_LENGTH_GET,
};


sw_error_t
fal_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_config_t * shaper);

sw_error_t
fal_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_config_t * shaper);

sw_error_t
fal_queue_shaper_set(a_uint32_t dev_id,a_uint32_t queue_id,
		fal_shaper_config_t * shaper);

sw_error_t
fal_queue_shaper_get(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_config_t * shaper);

sw_error_t
fal_flow_shaper_set(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_config_t * shaper);

sw_error_t
fal_flow_shaper_get(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_config_t * shaper);

sw_error_t
fal_queue_shaper_token_number_set(a_uint32_t dev_id,a_uint32_t queue_id,
		fal_shaper_token_number_t *token_number);

sw_error_t
fal_queue_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_token_number_t *token_number);

sw_error_t
fal_flow_shaper_token_number_set(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t *token_number);

sw_error_t
fal_flow_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t *token_number);

sw_error_t
fal_port_shaper_token_number_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t *token_number);

sw_error_t
fal_port_shaper_token_number_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t *token_number);

sw_error_t
fal_port_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot);

sw_error_t
fal_port_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot);

sw_error_t
fal_queue_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot);

sw_error_t
fal_queue_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot);

sw_error_t
fal_flow_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot);

sw_error_t
fal_flow_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot);

sw_error_t
fal_shaper_ipg_preamble_length_set(a_uint32_t dev_id, a_uint32_t ipg_pre_length);

sw_error_t
fal_shaper_ipg_preamble_length_get(a_uint32_t dev_id, a_uint32_t *ipg_pre_length);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_SHAPER_H_ */
/**
 * @}
 */
