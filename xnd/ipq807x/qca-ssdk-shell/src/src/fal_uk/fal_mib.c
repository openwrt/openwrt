/*
 * Copyright (c) 2014, 2017, The Linux Foundation. All rights reserved.
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
#include "fal_mib.h"
#include "fal_uk_if.h"

sw_error_t
fal_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                 fal_mib_info_t * mib_Info)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_MIB_GET, dev_id, port_id, mib_Info);
    return rv;
}

sw_error_t
fal_get_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                 fal_xgmib_info_t * mib_Info)
{
    sw_error_t rv;
    rv = sw_uk_exec(SW_API_PT_XGMIB_GET, dev_id, port_id, mib_Info);
    return rv;
}
sw_error_t
fal_mib_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIB_STATUS_SET, dev_id, (a_uint32_t)enable);
    return rv;
}

sw_error_t
fal_mib_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIB_STATUS_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_MIB_FLUSH_COUNTERS, dev_id, port_id);
    return rv;
}

sw_error_t
fal_mib_cpukeep_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIB_CPU_KEEP_SET, dev_id, (a_uint32_t)enable);
    return rv;
}

sw_error_t
fal_mib_cpukeep_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MIB_CPU_KEEP_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_mib_counter_get(a_uint32_t dev_id, fal_port_t port_id,
                 fal_mib_counter_t * mib_Info)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_MIB_COUNTER_GET, dev_id, port_id, mib_Info);
    return rv;
}
