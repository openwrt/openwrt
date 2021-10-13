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
#include "fal_flow.h"
#include "fal_uk_if.h"


sw_error_t
fal_flow_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_STATUS_SET, dev_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_flow_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_STATUS_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_flow_age_timer_set(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_AGE_TIMER_SET, dev_id, age_timer);
    return rv;
}

sw_error_t
fal_flow_age_timer_get(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_AGE_TIMER_GET, dev_id, age_timer);
    return rv;
}

sw_error_t
fal_flow_mgmt_set(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *mgmt)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_CTRL_SET, dev_id, type, dir, mgmt);
    return rv;
}

sw_error_t
fal_flow_mgmt_get(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *mgmt)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_CTRL_GET, dev_id, type, dir, mgmt);
    return rv;
}

sw_error_t
fal_flow_entry_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode, /*index or hash*/
		fal_flow_entry_t *flow_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_ENTRY_ADD, dev_id, add_mode, flow_entry);
    return rv;
}

sw_error_t
fal_flow_entry_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_entry_t *flow_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_ENTRY_DEL, dev_id, del_mode, flow_entry);
    return rv;
}

sw_error_t
fal_flow_entry_next(
		a_uint32_t dev_id,
		a_uint32_t next_mode,
		fal_flow_entry_t *flow_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOWENTRY_NEXT, dev_id, next_mode, flow_entry);
    return rv;
}

sw_error_t
fal_flow_entry_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_entry_t *flow_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_ENTRY_GET, dev_id, get_mode, flow_entry);
    return rv;
}

sw_error_t
fal_flow_global_cfg_get(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_GLOBAL_CFG_GET, dev_id, cfg);
    return rv;
}

sw_error_t
fal_flow_global_cfg_set(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_GLOBAL_CFG_SET, dev_id, cfg);
    return rv;
}

sw_error_t
fal_flow_host_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode, /*index or hash*/
		fal_flow_host_entry_t *flow_host_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_HOST_ADD, dev_id, add_mode, flow_host_entry);
    return rv;
}

sw_error_t
fal_flow_host_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_host_entry_t *flow_host_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_HOST_DEL, dev_id, del_mode, flow_host_entry);
    return rv;
}

sw_error_t
fal_flow_host_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_host_entry_t *flow_host_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_HOST_GET, dev_id, get_mode, flow_host_entry);
    return rv;
}

