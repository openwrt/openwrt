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
#include "fal_igmp.h"
#include "fal_uk_if.h"

sw_error_t
fal_port_igmps_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_IGMPS_MODE_SET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_igmps_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_IGMPS_MODE_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_MLD_CMD_SET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_igmp_mld_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_MLD_CMD_GET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_port_igmp_mld_join_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_PT_JOIN_SET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_igmp_mld_join_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_PT_JOIN_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_igmp_mld_leave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_PT_LEAVE_SET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_igmp_mld_leave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_PT_LEAVE_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_rp_set(a_uint32_t dev_id, fal_pbmp_t pts)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_RP_SET, dev_id, pts);
    return rv;
}

sw_error_t
fal_igmp_mld_rp_get(a_uint32_t dev_id, fal_pbmp_t * pts)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_RP_GET, dev_id, pts);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_creat_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_CREAT_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_creat_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_CREAT_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_static_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_STATIC_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_static_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_STATIC_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_leaky_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_LEAKY_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_leaky_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_LEAKY_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_v3_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_V3_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_v3_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_V3_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_queue_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_QUEUE_SET, dev_id, enable, queue);
    return rv;
}

sw_error_t
fal_igmp_mld_entry_queue_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_ENTRY_QUEUE_GET, dev_id, enable, queue);
    return rv;
}

sw_error_t
fal_port_igmp_mld_learn_limit_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_IGMP_LEARN_LIMIT_SET, dev_id, port_id, enable, cnt);
    return rv;
}

sw_error_t
fal_port_igmp_mld_learn_limit_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_IGMP_LEARN_LIMIT_GET, dev_id, port_id, enable, cnt);
    return rv;
}

sw_error_t
fal_port_igmp_mld_learn_exceed_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                                       fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_IGMP_LEARN_EXCEED_CMD_SET, dev_id, port_id, cmd);
    return rv;
}

sw_error_t
fal_port_igmp_mld_learn_exceed_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                                       fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_IGMP_LEARN_EXCEED_CMD_GET, dev_id, port_id, cmd);
    return rv;
}

sw_error_t
fal_igmp_sg_entry_set(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_SG_ENTRY_SET, dev_id, entry);
    return rv;
}

sw_error_t
fal_igmp_sg_entry_clear(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_SG_ENTRY_CLEAR, dev_id, entry);
    return rv;
}

sw_error_t
fal_igmp_sg_entry_show(a_uint32_t dev_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_SG_ENTRY_SHOW, dev_id);
    return rv;
}

sw_error_t
fal_igmp_sg_entry_query(a_uint32_t dev_id, fal_igmp_sg_info_t * info)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IGMP_SG_ENTRY_QUERY, dev_id, info);
    return rv;
}

