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
#include "fal_vlan.h"
#include "fal_uk_if.h"

sw_error_t
fal_vlan_entry_append(a_uint32_t dev_id, fal_vlan_t * vlan_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_APPEND, dev_id, vlan_entry);
    return rv;
}


sw_error_t
fal_vlan_create(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_ADD, dev_id, vlan_id);
    return rv;
}

sw_error_t
fal_vlan_next(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_NEXT, dev_id, vlan_id, p_vlan);
    return rv;
}

sw_error_t
fal_vlan_find(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_FIND, dev_id, vlan_id, p_vlan);
    return rv;
}

sw_error_t
fal_vlan_member_update(a_uint32_t dev_id, a_uint32_t vlan_id,
                       fal_pbmp_t member, fal_pbmp_t u_member)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_MEM_UPDATE, dev_id, vlan_id, member,
                    u_member);
    return rv;
}

sw_error_t
fal_vlan_delete(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_DEL, dev_id, vlan_id);
    return rv;
}

sw_error_t
fal_vlan_flush(a_uint32_t dev_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_FLUSH, dev_id);
    return rv;
}

sw_error_t
fal_vlan_fid_set(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t fid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_FID_SET, dev_id, vlan_id, fid);
    return rv;
}

sw_error_t
fal_vlan_fid_get(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t * fid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_FID_GET, dev_id, vlan_id, fid);
    return rv;
}

sw_error_t
fal_vlan_member_add(a_uint32_t dev_id, a_uint32_t vlan_id,
                    fal_port_t port_id, fal_pt_1q_egmode_t port_info)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_MEMBER_ADD, dev_id, vlan_id, port_id, port_info);
    return rv;
}

sw_error_t
fal_vlan_member_del(a_uint32_t dev_id, a_uint32_t vlan_id, fal_port_t port_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_MEMBER_DEL, dev_id, vlan_id, port_id);
    return rv;
}

sw_error_t
fal_vlan_learning_state_set(a_uint32_t dev_id, a_uint32_t vlan_id,
                            a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_LEARN_STATE_SET, dev_id, vlan_id, enable);
    return rv;
}

sw_error_t
fal_vlan_learning_state_get(a_uint32_t dev_id, a_uint32_t vlan_id,
                            a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VLAN_LEARN_STATE_GET, dev_id, vlan_id, enable);
    return rv;
}


