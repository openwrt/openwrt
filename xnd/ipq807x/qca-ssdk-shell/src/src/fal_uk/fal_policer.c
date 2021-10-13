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
#include "fal_policer.h"
#include "fal_uk_if.h"


sw_error_t
fal_policer_timeslot_set(a_uint32_t dev_id, a_uint32_t timeslot)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_TIMESLOT_SET, dev_id, timeslot);
    return rv;
}

sw_error_t
fal_policer_timeslot_get(a_uint32_t dev_id, a_uint32_t *timeslot)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_TIMESLOT_GET, dev_id, timeslot);
    return rv;
}

sw_error_t
fal_port_policer_counter_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_counter_t *counter)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_PORT_COUNTER_GET, dev_id, port_id,
		counter);
    return rv;
}

sw_error_t
fal_acl_policer_counter_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_counter_t *counter)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_ACL_COUNTER_GET, dev_id, index,
		counter);
    return rv;
}

sw_error_t
fal_port_policer_compensation_byte_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t length)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_COMPENSATION_SET, dev_id, port_id, length);
    return rv;
}

sw_error_t
fal_port_policer_compensation_byte_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *length)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_COMPENSATION_GET, dev_id, port_id,
		 length);
    return rv;
}

sw_error_t
fal_port_policer_entry_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_PORT_ENTRY_SET, dev_id, port_id,
		policer, action);
    return rv;
}

sw_error_t
fal_port_policer_entry_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_PORT_ENTRY_GET, dev_id, port_id,
		policer, action);
    return rv;
}

sw_error_t
fal_acl_policer_entry_set(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_ACL_ENTRY_SET, dev_id, index,
		policer, action);
    return rv;
}

sw_error_t
fal_acl_policer_entry_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_ACL_ENTRY_GET, dev_id, index,
		policer, action);
    return rv;
}

sw_error_t
fal_policer_global_counter_get(a_uint32_t dev_id,fal_policer_global_counter_t *counter)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_POLICER_GLOBAL_COUNTER_GET, dev_id,
		counter);
    return rv;
}


