/*
 * Copyright (c) 2014, 2015, The Linux Foundation. All rights reserved.
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
#include "fal_nat.h"
#include "fal_uk_if.h"

sw_error_t
fal_nat_add(a_uint32_t dev_id, fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_ADD, dev_id, nat_entry);
    return rv;
}


sw_error_t
fal_nat_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_DEL, dev_id, del_mode, nat_entry);
    return rv;
}


sw_error_t
fal_nat_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_GET, dev_id, get_mode, nat_entry);
    return rv;
}

sw_error_t
fal_nat_next(a_uint32_t dev_id, a_uint32_t next_mode, fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_NEXT, dev_id, next_mode, nat_entry);
    return rv;
}

sw_error_t
fal_nat_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_COUNTER_BIND, dev_id, entry_id, cnt_id, enable);
    return rv;
}


sw_error_t
fal_napt_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAPT_ADD, dev_id, napt_entry);
    return rv;
}

sw_error_t
fal_napt_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAPT_DEL, dev_id, del_mode, napt_entry);
    return rv;
}

sw_error_t
fal_napt_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAPT_GET, dev_id, get_mode, napt_entry);
    return rv;
}

sw_error_t
fal_napt_next(a_uint32_t dev_id, a_uint32_t next_mode, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAPT_NEXT, dev_id, next_mode, napt_entry);
    return rv;
}

sw_error_t
fal_napt_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAPT_COUNTER_BIND, dev_id, entry_id, cnt_id, enable);
    return rv;
}

sw_error_t
fal_flow_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_ADD, dev_id, napt_entry);
    return rv;
}

sw_error_t
fal_flow_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_DEL, dev_id, del_mode, napt_entry);
    return rv;
}

sw_error_t
fal_flow_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_GET, dev_id, get_mode, napt_entry);
    return rv;
}

sw_error_t
fal_flow_next(a_uint32_t dev_id, a_uint32_t next_mode, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_NEXT, dev_id, next_mode, napt_entry);
    return rv;
}

sw_error_t
fal_flow_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_COUNTER_BIND, dev_id, entry_id, cnt_id, enable);
    return rv;
}

sw_error_t
fal_nat_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_STATUS_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_nat_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_STATUS_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_nat_hash_mode_set(a_uint32_t dev_id, a_uint32_t mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_HASH_MODE_SET, dev_id, mode);
    return rv;
}

sw_error_t
fal_nat_hash_mode_get(a_uint32_t dev_id, a_uint32_t * mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_HASH_MODE_GET, dev_id, mode);
    return rv;
}

sw_error_t
fal_napt_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAPT_STATUS_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_napt_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAPT_STATUS_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_napt_mode_set(a_uint32_t dev_id, fal_napt_mode_t mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAPT_MODE_SET, dev_id, mode);
    return rv;
}

sw_error_t
fal_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAPT_MODE_GET, dev_id, mode);
    return rv;
}

sw_error_t
fal_nat_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PRV_BASE_ADDR_SET, dev_id, addr);
    return rv;
}

sw_error_t
fal_nat_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PRV_BASE_ADDR_GET, dev_id, addr);
    return rv;
}

sw_error_t
fal_nat_prv_base_mask_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PRV_BASE_MASK_SET, dev_id, addr);
    return rv;
}

sw_error_t
fal_nat_prv_base_mask_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PRV_BASE_MASK_GET, dev_id, addr);
    return rv;
}

sw_error_t
fal_nat_prv_addr_mode_set(a_uint32_t dev_id, a_bool_t map_en)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PRV_ADDR_MODE_SET, dev_id, map_en);
    return rv;
}

sw_error_t
fal_nat_prv_addr_mode_get(a_uint32_t dev_id, a_bool_t * map_en)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PRV_ADDR_MODE_GET, dev_id, map_en);
    return rv;
}

sw_error_t
fal_nat_pub_addr_add(a_uint32_t dev_id, fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PUB_ADDR_ENTRY_ADD, dev_id, entry);
    return rv;
}

sw_error_t
fal_nat_pub_addr_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PUB_ADDR_ENTRY_DEL, dev_id, del_mode, entry);
    return rv;
}

sw_error_t
fal_nat_pub_addr_next(a_uint32_t dev_id, a_uint32_t next_mode, fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PUB_ADDR_ENTRY_NEXT, dev_id, next_mode, entry);
    return rv;
}

sw_error_t
fal_nat_unk_session_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_UNK_SESSION_CMD_SET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_nat_unk_session_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_UNK_SESSION_CMD_GET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_nat_global_set(a_uint32_t dev_id, a_bool_t enable,
	a_bool_t sync_cnt_enable, a_uint32_t portbmp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NAT_GLOBAL_SET, dev_id, enable,
    	 sync_cnt_enable, portbmp);
    return rv;
}

sw_error_t
fal_flow_cookie_set(a_uint32_t dev_id, fal_flow_cookie_t * flow_cookie)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_COOKIE_SET, dev_id, flow_cookie);
    return rv;
}

sw_error_t
fal_flow_rfs_set(a_uint32_t dev_id, a_uint8_t action, fal_flow_rfs_t * rfs)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FLOW_RFS_SET, dev_id, action, rfs);
    return rv;
}


