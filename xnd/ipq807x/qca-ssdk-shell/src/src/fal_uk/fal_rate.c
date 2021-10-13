/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
#include "fal_rate.h"
#include "fal_uk_if.h"

sw_error_t
fal_rate_queue_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_queue_t queue_id, a_uint32_t * speed,
                        a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_QU_EGRL_SET, dev_id, port_id, queue_id,
                    speed, enable);
    return rv;
}

sw_error_t
fal_rate_queue_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_queue_t queue_id, a_uint32_t * speed,
                        a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_QU_EGRL_GET, dev_id, port_id, queue_id,
                    speed, enable);
    return rv;
}

sw_error_t
fal_rate_port_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                       a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PT_EGRL_SET, dev_id, port_id,
                    speed, enable);
    return rv;
}

sw_error_t
fal_rate_port_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                       a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PT_EGRL_GET, dev_id, port_id,
                    speed, enable);
    return rv;
}

sw_error_t
fal_rate_port_inrl_set(a_uint32_t dev_id, fal_port_t port_id,
                       a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PT_INRL_SET, dev_id, port_id,
                    speed, enable);
    return rv;
}

sw_error_t
fal_rate_port_inrl_get(a_uint32_t dev_id, fal_port_t port_id,
                       a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PT_INRL_GET, dev_id, port_id,
                    speed, enable);
    return rv;
}

sw_error_t
fal_storm_ctrl_frame_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_storm_type_t frame_type, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_STORM_CTRL_FRAME_SET, dev_id, port_id,
                    frame_type, enable);
    return rv;
}

sw_error_t
fal_storm_ctrl_frame_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_storm_type_t frame_type, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_STORM_CTRL_FRAME_GET, dev_id, port_id,
                    frame_type, enable);
    return rv;
}

sw_error_t
fal_storm_ctrl_rate_set(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * rate)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_STORM_CTRL_RATE_SET, dev_id, port_id,
                    rate);
    return rv;
}

sw_error_t
fal_storm_ctrl_rate_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * rate)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_STORM_CTRL_RATE_GET, dev_id, port_id,
                    rate);
    return rv;
}

sw_error_t
fal_rate_port_policer_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_port_policer_t * policer)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PORT_POLICER_SET, dev_id, port_id,
                    policer);
    return rv;
}

sw_error_t
fal_rate_port_policer_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_port_policer_t * policer)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PORT_POLICER_GET, dev_id, port_id,
                    policer);
    return rv;
}

sw_error_t
fal_rate_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PORT_SHAPER_SET, dev_id, port_id,
                    enable, shaper);
    return rv;
}

sw_error_t
fal_rate_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PORT_SHAPER_GET, dev_id, port_id,
                    enable, shaper);
    return rv;
}

sw_error_t
fal_rate_queue_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_queue_t queue_id, a_bool_t enable,
                          fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_QUEUE_SHAPER_SET, dev_id, port_id, queue_id,
                    enable, shaper);
    return rv;
}

sw_error_t
fal_rate_queue_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_queue_t queue_id, a_bool_t * enable,
                          fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_QUEUE_SHAPER_GET, dev_id, port_id, queue_id,
                    enable, shaper);
    return rv;
}

sw_error_t
fal_rate_acl_policer_set(a_uint32_t dev_id, a_uint32_t policer_id,
                         fal_acl_policer_t * policer)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_ACL_POLICER_SET, dev_id, policer_id, policer);
    return rv;
}

sw_error_t
fal_rate_acl_policer_get(a_uint32_t dev_id, a_uint32_t policer_id,
                         fal_acl_policer_t * policer)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_ACL_POLICER_GET, dev_id, policer_id, policer);
    return rv;
}

sw_error_t
fal_rate_port_add_rate_byte_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t  number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PT_ADDRATEBYTE_SET, dev_id, port_id, number);
    return rv;
}

sw_error_t
fal_rate_port_add_rate_byte_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t  *number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PT_ADDRATEBYTE_GET, dev_id, port_id, number);
    return rv;
}

sw_error_t
fal_rate_port_gol_flow_en_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t  enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PT_GOL_FLOW_EN_SET, dev_id, port_id, enable);
    return rv;
}


sw_error_t
fal_rate_port_gol_flow_en_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t*  enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RATE_PT_GOL_FLOW_EN_GET, dev_id, port_id, enable);
    return rv;
}


