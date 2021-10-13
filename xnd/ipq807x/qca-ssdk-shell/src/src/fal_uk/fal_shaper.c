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
#include "fal_shaper.h"
#include "fal_uk_if.h"


sw_error_t
fal_port_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_SHAPER_TIMESLOT_SET, dev_id, timeslot);
    return rv;
}

sw_error_t
fal_port_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_SHAPER_TIMESLOT_GET, dev_id, timeslot);
    return rv;
}

sw_error_t
fal_flow_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_SHAPER_TIMESLOT_SET, dev_id, timeslot);
    return rv;
}

sw_error_t
fal_flow_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_SHAPER_TIMESLOT_GET, dev_id, timeslot);
    return rv;
}

sw_error_t
fal_queue_shaper_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_SHAPER_TIMESLOT_SET, dev_id, timeslot);
    return rv;
}

sw_error_t
fal_queue_shaper_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_SHAPER_TIMESLOT_GET, dev_id, timeslot);
    return rv;
}

sw_error_t
fal_port_shaper_token_number_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t * token_number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_SHAPER_TOKEN_NUMBER_SET, dev_id, port_id,
		token_number);
    return rv;
}

sw_error_t
fal_port_shaper_token_number_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t * token_number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_SHAPER_TOKEN_NUMBER_GET, dev_id, port_id,
		token_number);
    return rv;
}

sw_error_t
fal_flow_shaper_token_number_set(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t * token_number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_SHAPER_TOKEN_NUMBER_SET, dev_id, flow_id,
		token_number);
    return rv;
}

sw_error_t
fal_flow_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t * token_number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_SHAPER_TOKEN_NUMBER_GET, dev_id, flow_id,
		token_number);
    return rv;
}

sw_error_t
fal_queue_shaper_token_number_set(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_token_number_t * token_number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_SHAPER_TOKEN_NUMBER_SET, dev_id, queue_id,
		token_number);
    return rv;
}

sw_error_t
fal_queue_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_token_number_t * token_number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_SHAPER_TOKEN_NUMBER_GET, dev_id, queue_id,
		token_number);
    return rv;
}

sw_error_t
fal_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id, fal_shaper_config_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_SHAPER_SET, dev_id, port_id,
		shaper);
    return rv;
}

sw_error_t
fal_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id, fal_shaper_config_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_SHAPER_GET, dev_id, port_id,
		shaper);
    return rv;
}

sw_error_t
fal_flow_shaper_set(a_uint32_t dev_id, a_uint32_t flow_id, fal_shaper_config_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_SHAPER_SET, dev_id, flow_id,
		shaper);
    return rv;
}

sw_error_t
fal_flow_shaper_get(a_uint32_t dev_id, a_uint32_t flow_id, fal_shaper_config_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_SHAPER_GET, dev_id, flow_id,
		shaper);
    return rv;
}

sw_error_t
fal_queue_shaper_set(a_uint32_t dev_id, a_uint32_t queue_id, fal_shaper_config_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_SHAPER_SET, dev_id, queue_id,
		shaper);
    return rv;
}

sw_error_t
fal_queue_shaper_get(a_uint32_t dev_id, a_uint32_t queue_id, fal_shaper_config_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QUEUE_SHAPER_GET, dev_id, queue_id,
		shaper);
    return rv;
}

sw_error_t
fal_shaper_ipg_preamble_length_set(a_uint32_t dev_id, a_uint32_t ipg_pre_length)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SHAPER_IPG_PRE_SET, dev_id, ipg_pre_length);
    return rv;
}

sw_error_t
fal_shaper_ipg_preamble_length_get(a_uint32_t dev_id, a_uint32_t *ipg_pre_length)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SHAPER_IPG_PRE_GET, dev_id, ipg_pre_length);
    return rv;
}


