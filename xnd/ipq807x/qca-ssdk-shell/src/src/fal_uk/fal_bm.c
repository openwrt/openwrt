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
#include "fal_bm.h"
#include "fal_uk_if.h"

sw_error_t
fal_port_bufgroup_map_get(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t *group)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_PORTGROUP_MAP_GET, dev_id, port, group);
    return rv;
}

sw_error_t
fal_port_bufgroup_map_set(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t group)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_PORTGROUP_MAP_SET, dev_id, port, group);
    return rv;
}

sw_error_t
fal_bm_port_reserved_buffer_get(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t *prealloc_buff, a_uint16_t *react_buff)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_PORT_RSVBUFFER_GET, dev_id, port,
    			prealloc_buff, react_buff);
    return rv;
}

sw_error_t
fal_bm_port_reserved_buffer_set(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t prealloc_buff, a_uint16_t react_buff)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_PORT_RSVBUFFER_SET, dev_id, port,
    			prealloc_buff, react_buff);
    return rv;
}

sw_error_t
fal_bm_bufgroup_buffer_get(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t *buff_num)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_GROUP_BUFFER_GET, dev_id, group,
    			buff_num);
    return rv;
}

sw_error_t
fal_bm_bufgroup_buffer_set(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t buff_num)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_GROUP_BUFFER_SET, dev_id, group,
    			buff_num);
    return rv;
}

sw_error_t
fal_bm_port_dynamic_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_DYNAMIC_THRESH_GET, dev_id, port,
    			cfg);
    return rv;
}

sw_error_t
fal_bm_port_dynamic_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_DYNAMIC_THRESH_SET, dev_id, port,
    			cfg);
    return rv;
}

sw_error_t
fal_port_bm_ctrl_get(a_uint32_t dev_id, fal_port_t port, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_CTRL_GET, dev_id, port,
    			enable);
    return rv;
}

sw_error_t
fal_port_bm_ctrl_set(a_uint32_t dev_id, fal_port_t port, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_CTRL_SET, dev_id, port,
    			enable);
    return rv;
}

sw_error_t
fal_bm_port_static_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_STATIC_THRESH_GET, dev_id, port,
    			cfg);
    return rv;
}

sw_error_t
fal_bm_port_static_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_STATIC_THRESH_SET, dev_id, port,
    			cfg);
    return rv;
}

sw_error_t
fal_bm_port_counter_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_port_counter_t *counter)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BM_PORT_COUNTER_GET, dev_id, port,
    			counter);
    return rv;
}

