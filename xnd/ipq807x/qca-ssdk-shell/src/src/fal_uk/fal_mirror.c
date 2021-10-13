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



#include "sw.h"
#include "sw_ioctl.h"
#include "fal_mirror.h"
#include "fal_uk_if.h"

sw_error_t
fal_mirr_analysis_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIRROR_ANALY_PT_SET, dev_id, port_id);
    return rv;
}

sw_error_t
fal_mirr_analysis_port_get(a_uint32_t dev_id, fal_port_t * port_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIRROR_ANALY_PT_GET, dev_id,
                    port_id);
    return rv;
}

sw_error_t
fal_mirr_port_in_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIRROR_IN_PT_SET, dev_id, port_id,
                    (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_mirr_port_in_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIRROR_IN_PT_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_mirr_port_eg_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIRROR_EG_PT_SET, dev_id, port_id,
                    (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_mirr_port_eg_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIRROR_EG_PT_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_mirr_analysis_config_set(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIRROR_ANALYSIS_CONFIG_SET, dev_id, direction, config);
    return rv;
}

sw_error_t
fal_mirr_analysis_config_get(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIRROR_ANALYSIS_CONFIG_GET, dev_id, direction, config);
    return rv;
}

