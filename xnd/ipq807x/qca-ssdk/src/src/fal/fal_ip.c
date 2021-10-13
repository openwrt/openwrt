/*
 * Copyright (c) 2012, 2015, 2017, The Linux Foundation. All rights reserved.
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


/**
 * @defgroup fal_ip FAL_IP
 * @{
 */
#include "sw.h"
#include "fal_ip.h"
#include "hsl_api.h"
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>

#ifndef IN_IP_MINI
static sw_error_t
_fal_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * host_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_ip_host_add)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_ip_host_add(dev_id, host_entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_host_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_host_add(dev_id, host_entry);
    return rv;
}

static sw_error_t
_fal_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode,
                 fal_host_entry_t * host_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_ip_host_del)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_ip_host_del(dev_id, del_mode, host_entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_host_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_host_del(dev_id, del_mode, host_entry);
    return rv;
}

static sw_error_t
_fal_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode,
                 fal_host_entry_t * host_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_ip_host_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_ip_host_get(dev_id, get_mode, host_entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_host_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_host_get(dev_id, get_mode, host_entry);
    return rv;
}

static sw_error_t
_fal_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode,
                  fal_host_entry_t * host_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_ip_host_next)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_ip_host_next(dev_id, next_mode, host_entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_host_next)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_host_next(dev_id, next_mode, host_entry);
    return rv;
}

static sw_error_t
_fal_ip_host_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                          a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_host_counter_bind)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_host_counter_bind(dev_id, entry_id, cnt_id, enable);
    return rv;
}

static sw_error_t
_fal_ip_host_pppoe_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                        a_uint32_t pppoe_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_host_pppoe_bind)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_host_pppoe_bind(dev_id, entry_id, pppoe_id, enable);
    return rv;
}

static sw_error_t
_fal_ip_pt_arp_learn_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t flags)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_pt_arp_learn_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_pt_arp_learn_set(dev_id, port_id, flags);
    return rv;
}

static sw_error_t
_fal_ip_pt_arp_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * flags)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_pt_arp_learn_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_pt_arp_learn_get(dev_id, port_id, flags);
    return rv;
}

static sw_error_t
_fal_ip_arp_learn_set(a_uint32_t dev_id, fal_arp_learn_mode_t mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_arp_learn_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_arp_learn_set(dev_id, mode);
    return rv;
}

static sw_error_t
_fal_ip_arp_learn_get(a_uint32_t dev_id, fal_arp_learn_mode_t * mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_arp_learn_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_arp_learn_get(dev_id, mode);
    return rv;
}

static sw_error_t
_fal_ip_source_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_source_guard_mode_t mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_source_guard_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_source_guard_set(dev_id, port_id, mode);
    return rv;
}

static sw_error_t
_fal_ip_source_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_source_guard_mode_t * mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_source_guard_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_source_guard_get(dev_id, port_id, mode);
    return rv;
}

static sw_error_t
_fal_ip_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_unk_source_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_unk_source_cmd_set(dev_id, cmd);
    return rv;
}

static sw_error_t
_fal_ip_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_unk_source_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_unk_source_cmd_get(dev_id, cmd);
    return rv;
}

static sw_error_t
_fal_ip_arp_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_source_guard_mode_t mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_arp_guard_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_arp_guard_set(dev_id, port_id, mode);
    return rv;
}

static sw_error_t
_fal_ip_arp_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_source_guard_mode_t * mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_arp_guard_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_arp_guard_get(dev_id, port_id, mode);
    return rv;
}

static sw_error_t
_fal_arp_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->arp_unk_source_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->arp_unk_source_cmd_set(dev_id, cmd);
    return rv;
}

static sw_error_t
_fal_arp_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->arp_unk_source_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->arp_unk_source_cmd_get(dev_id, cmd);
    return rv;
}

static sw_error_t
_fal_ip_route_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_route_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_route_status_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_ip_route_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_route_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_route_status_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_ip_intf_entry_add(a_uint32_t dev_id, fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_intf_entry_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_intf_entry_add(dev_id, entry);
    return rv;
}

static sw_error_t
_fal_ip_intf_entry_del(a_uint32_t dev_id, a_uint32_t del_mode,
                       fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_intf_entry_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_intf_entry_del(dev_id, del_mode, entry);
    return rv;
}

static sw_error_t
_fal_ip_intf_entry_next(a_uint32_t dev_id, a_uint32_t next_mode,
                        fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_intf_entry_next)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_intf_entry_next(dev_id, next_mode, entry);
    return rv;
}

static sw_error_t
_fal_ip_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_age_time_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_age_time_set(dev_id, time);
    return rv;
}

static sw_error_t
_fal_ip_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_age_time_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_age_time_get(dev_id, time);
    return rv;
}

static sw_error_t
_fal_ip_wcmp_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_wcmp_hash_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_wcmp_hash_mode_set(dev_id, hash_mode);
    return rv;
}

static sw_error_t
_fal_ip_wcmp_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_wcmp_hash_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_wcmp_hash_mode_get(dev_id, hash_mode);
    return rv;
}

static sw_error_t
_fal_ip_vrf_base_addr_set(a_uint32_t dev_id,
					a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_vrf_base_addr_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_vrf_base_addr_set(dev_id, vrf_id, addr);
    return rv;
}

static sw_error_t
_fal_ip_vrf_base_addr_get(a_uint32_t dev_id,
					a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_vrf_base_addr_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_vrf_base_addr_get(dev_id, vrf_id, addr);
    return rv;
}

static sw_error_t
_fal_ip_vrf_base_mask_set(a_uint32_t dev_id,
					a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_vrf_base_mask_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_vrf_base_mask_set(dev_id, vrf_id, addr);
    return rv;
}

static sw_error_t
_fal_ip_vrf_base_mask_get(a_uint32_t dev_id,
					a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_vrf_base_mask_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_vrf_base_mask_get(dev_id, vrf_id, addr);
    return rv;
}

static sw_error_t
_fal_ip_default_route_set(a_uint32_t dev_id,
					a_uint32_t droute_id, fal_default_route_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_default_route_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_default_route_set(dev_id, droute_id, entry);
    return rv;
}

static sw_error_t
_fal_ip_default_route_get(a_uint32_t dev_id,
					a_uint32_t droute_id, fal_default_route_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_default_route_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_default_route_get(dev_id, droute_id, entry);
    return rv;
}

static sw_error_t
_fal_ip_host_route_set(a_uint32_t dev_id,
					a_uint32_t hroute_id, fal_host_route_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_host_route_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_host_route_set(dev_id, hroute_id, entry);
    return rv;
}

static sw_error_t
_fal_ip_host_route_get(a_uint32_t dev_id,
					a_uint32_t hroute_id, fal_host_route_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_host_route_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_host_route_get(dev_id, hroute_id, entry);
    return rv;
}

static sw_error_t
_fal_ip_wcmp_entry_set(a_uint32_t dev_id,
					a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_wcmp_entry_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_wcmp_entry_set(dev_id, wcmp_id, wcmp);
    return rv;
}

static sw_error_t
_fal_ip_wcmp_entry_get(a_uint32_t dev_id,
					a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_wcmp_entry_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_wcmp_entry_get(dev_id, wcmp_id, wcmp);
    return rv;
}

static sw_error_t
_fal_ip_rfs_ip4_rule_set(a_uint32_t dev_id, fal_ip4_rfs_t * rfs)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_rfs_ip4_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_rfs_ip4_set(dev_id, rfs);
    return rv;
}

static sw_error_t
_fal_ip_rfs_ip6_rule_set(a_uint32_t dev_id, fal_ip6_rfs_t * rfs)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_rfs_ip6_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_rfs_ip6_set(dev_id, rfs);
    return rv;
}

static sw_error_t
_fal_ip_rfs_ip4_rule_del(a_uint32_t dev_id, fal_ip4_rfs_t * rfs)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_rfs_ip4_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_rfs_ip4_del(dev_id, rfs);
    return rv;
}

static sw_error_t
_fal_ip_rfs_ip6_rule_del(a_uint32_t dev_id, fal_ip6_rfs_t * rfs)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_rfs_ip6_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_rfs_ip6_del(dev_id, rfs);
    return rv;
}

static sw_error_t
_fal_default_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_default_flow_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_default_flow_cmd_set(dev_id, vrf_id, type, cmd);
    return rv;
}

sw_error_t
_fal_default_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_default_flow_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_default_flow_cmd_get(dev_id, vrf_id, type, cmd);
    return rv;
}

sw_error_t
_fal_default_rt_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_default_rt_flow_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_default_rt_flow_cmd_set(dev_id, vrf_id, type, cmd);
    return rv;
}

sw_error_t
_fal_default_rt_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ip_default_rt_flow_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ip_default_rt_flow_cmd_get(dev_id, vrf_id, type, cmd);
    return rv;
}

sw_error_t
_fal_ip_network_route_get(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type,
			fal_network_route_entry_t *entry)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_network_route_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_network_route_get(dev_id, index, type, entry);
    return rv;
}
sw_error_t
_fal_ip_vsi_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_vsi_sg_cfg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_vsi_sg_cfg_get(dev_id, vsi, sg_cfg);
    return rv;
}

sw_error_t
_fal_ip_network_route_del(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_network_route_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_network_route_del(dev_id, index, type);
    return rv;
}
sw_error_t
_fal_ip_port_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_port_sg_cfg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_port_sg_cfg_set(dev_id, port_id, sg_cfg);
    return rv;
}
sw_error_t
_fal_ip_port_intf_get(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_port_intf_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_port_intf_get(dev_id, port_id, id);
    return rv;
}
sw_error_t
_fal_ip_vsi_arp_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_vsi_arp_sg_cfg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_vsi_arp_sg_cfg_set(dev_id, vsi, arp_sg_cfg);
    return rv;
}
sw_error_t
_fal_ip_pub_addr_get(a_uint32_t dev_id,
		a_uint32_t index, fal_ip_pub_addr_t *entry)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_pub_addr_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_pub_addr_get(dev_id, index, entry);
    return rv;
}
sw_error_t
_fal_ip_port_intf_set(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_port_intf_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_port_intf_set(dev_id, port_id, id);
    return rv;
}
sw_error_t
_fal_ip_vsi_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_vsi_sg_cfg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_vsi_sg_cfg_set(dev_id, vsi, sg_cfg);
    return rv;
}
sw_error_t
_fal_ip_port_macaddr_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_port_macaddr_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_port_macaddr_set(dev_id, port_id, macaddr);
    return rv;
}
sw_error_t
_fal_ip_vsi_intf_get(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_vsi_intf_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_vsi_intf_get(dev_id, vsi, id);
    return rv;
}

sw_error_t
_fal_ip_network_route_add(a_uint32_t dev_id,
			a_uint32_t index,
			fal_network_route_entry_t *entry)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_network_route_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_network_route_add(dev_id, index, entry);
    return rv;
}
sw_error_t
_fal_ip_port_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_port_sg_cfg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_port_sg_cfg_get(dev_id, port_id, sg_cfg);
    return rv;
}
sw_error_t
_fal_ip_intf_get(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_intf_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_intf_get(dev_id, index, entry);
    return rv;
}
sw_error_t
_fal_ip_pub_addr_set(a_uint32_t dev_id,
		a_uint32_t index, fal_ip_pub_addr_t *entry)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_pub_addr_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_pub_addr_set(dev_id, index, entry);
    return rv;
}
sw_error_t
_fal_ip_route_mismatch_get(a_uint32_t dev_id, fal_fwd_cmd_t *cmd)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_route_mismatch_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_route_mismatch_get(dev_id, cmd);
    return rv;
}
sw_error_t
_fal_ip_vsi_arp_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_vsi_arp_sg_cfg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_vsi_arp_sg_cfg_get(dev_id, vsi, arp_sg_cfg);
    return rv;
}
sw_error_t
_fal_ip_port_arp_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_port_arp_sg_cfg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_port_arp_sg_cfg_set(dev_id, port_id, arp_sg_cfg);
    return rv;
}
sw_error_t
_fal_ip_vsi_mc_mode_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_vsi_mc_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_vsi_mc_mode_set(dev_id, vsi, cfg);
    return rv;
}
sw_error_t
_fal_ip_vsi_intf_set(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_vsi_intf_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_vsi_intf_set(dev_id, vsi, id);
    return rv;
}
sw_error_t
_fal_ip_nexthop_get(a_uint32_t dev_id, a_uint32_t index, fal_ip_nexthop_t *entry)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_nexthop_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_nexthop_get(dev_id, index, entry);
    return rv;
}
sw_error_t
_fal_ip_route_mismatch_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_route_mismatch_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_route_mismatch_set(dev_id, cmd);
    return rv;
}
sw_error_t
_fal_ip_intf_set(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_intf_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_intf_set(dev_id, index, entry);
    return rv;
}
sw_error_t
_fal_ip_vsi_mc_mode_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_vsi_mc_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_vsi_mc_mode_get(dev_id, vsi, cfg);
    return rv;
}
sw_error_t
_fal_ip_port_macaddr_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_port_macaddr_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_port_macaddr_get(dev_id, port_id, macaddr);
    return rv;
}
sw_error_t
_fal_ip_port_arp_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_port_arp_sg_cfg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_port_arp_sg_cfg_get(dev_id, port_id, arp_sg_cfg);
    return rv;
}
sw_error_t
_fal_ip_nexthop_set(a_uint32_t dev_id,
			a_uint32_t index, fal_ip_nexthop_t *entry)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_ip_nexthop_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_ip_nexthop_set(dev_id, index, entry);
    return rv;
}

sw_error_t
_fal_ip_global_ctrl_set(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ip_global_ctrl_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ip_global_ctrl_set(dev_id, cfg);
	return rv;
}

sw_error_t
_fal_ip_global_ctrl_get(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ip_global_ctrl_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ip_global_ctrl_get(dev_id, cfg);
	return rv;
}

/*insert flag for inner fal, don't remove it*/

/**
 * @brief Add one host entry to one particular device.
 *   @details Comments:
 *     For ISIS the intf_id parameter in host_entry means vlan id.
       Before host entry added related interface entry and ip6 base address
       must be set at first.
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] host_entry host entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_host_add(dev_id, host_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete one host entry from one particular device.
 *   @details Comments:
 *     For ISIS the intf_id parameter in host_entry means vlan id.
       Before host entry deleted related interface entry and ip6 base address
       must be set atfirst.
       For del_mode please refer IP entry operation flags.
 * @param[in] dev_id device id
 * @param[in] del_mode delete operation mode
 * @param[in] host_entry host entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode,
                fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_host_del(dev_id, del_mode, host_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get one host entry from one particular device.
 *   @details Comments:
 *     For ISIS the intf_id parameter in host_entry means vlan id.
       Before host entry deleted related interface entry and ip6 base address
       must be set atfirst.
       For get_mode please refer IP entry operation flags.
 * @param[in] dev_id device id
 * @param[in] get_mode get operation mode
 * @param[out] host_entry host entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode,
                fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_host_get(dev_id, get_mode, host_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next one host entry from one particular device.
 *   @details Comments:
 *     For ISIS the intf_id parameter in host_entry means vlan id.
       Before host entry deleted related interface entry and ip6 base address
       must be set atfirst.
       For next_mode please refer IP entry operation flags.
       For get the first entry please set entry id as FAL_NEXT_ENTRY_FIRST_ID
 * @param[in] dev_id device id
 * @param[in] next_mode next operation mode
 * @param[out] host_entry host entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode,
                 fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_host_next(dev_id, next_mode, host_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one counter entry to one host entry on one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id host entry id
 * @param[in] cnt_id counter entry id
 * @param[in] enable A_TRUE means bind, A_FALSE means unbind
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_host_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                         a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_host_counter_bind(dev_id, entry_id, cnt_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one pppoe session entry to one host entry on one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id host entry id
 * @param[in] pppoe_id pppoe session entry id
 * @param[in] enable A_TRUE means bind, A_FALSE means unbind
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_host_pppoe_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                       a_uint32_t pppoe_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_host_pppoe_bind(dev_id, entry_id, pppoe_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets type to learn on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] flags arp type FAL_ARP_LEARN_REQ and/or FAL_ARP_LEARN_ACK
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_pt_arp_learn_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t flags)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_pt_arp_learn_set(dev_id, port_id, flags);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets type to learn on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] flags arp type FAL_ARP_LEARN_REQ and/or FAL_ARP_LEARN_ACK
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_pt_arp_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * flags)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_pt_arp_learn_get(dev_id, port_id, flags);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets type to learn on one particular device.
 * @param[in] dev_id device id
 * @param[in] mode learning mode
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_arp_learn_set(a_uint32_t dev_id, fal_arp_learn_mode_t mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_arp_learn_set(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets type to learn on one particular device.
 * @param[in] dev_id device id
 * @param[out] mode learning mode
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_arp_learn_get(a_uint32_t dev_id, fal_arp_learn_mode_t * mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_arp_learn_get(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set ip packets source guarding mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode source guarding mode
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_source_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_source_guard_mode_t mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_source_guard_set(dev_id, port_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get ip packets source guarding mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode source guarding mode
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_source_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_source_guard_mode_t * mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_source_guard_get(dev_id, port_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set unkonw source ip packets forwarding command on one particular device.
 *   @details Comments:
 *     This settin is no meaning when ip source guard not enable
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_unk_source_cmd_set(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get unkonw source ip packets forwarding command on one particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_unk_source_cmd_get(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets source guarding mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode source guarding mode
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_arp_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                     fal_source_guard_mode_t mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_arp_guard_set(dev_id, port_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets source guarding mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode source guarding mode
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_arp_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                     fal_source_guard_mode_t * mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_arp_guard_get(dev_id, port_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set unkonw source arp packets forwarding command on one particular device.
 *   @details Comments:
 *     This settin is no meaning when arp source guard not enable
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_arp_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_arp_unk_source_cmd_set(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get unkonw source arp packets forwarding command on one particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_arp_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_arp_unk_source_cmd_get(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP unicast routing status on one particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_route_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_route_status_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP unicast routing status on one particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_route_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_route_status_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add one interface entry to one particular device.
 * @param[in] dev_id device id
 * @param[in] entry interface entry
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_intf_entry_add(a_uint32_t dev_id, fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_intf_entry_add(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete one interface entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode delete operation mode
 * @param[in] entry interface entry
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_intf_entry_del(a_uint32_t dev_id, a_uint32_t del_mode,
                      fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_intf_entry_del(dev_id, del_mode, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next one interface entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode next operation mode
 * @param[out] entry interface entry
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_intf_entry_next(a_uint32_t dev_id, a_uint32_t next_mode,
                       fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_intf_entry_next(dev_id, next_mode, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP host entry aging time on one particular device.
 * @details   Comments:
 *       This operation will set dynamic entry aging time on a particular device.
 *       The unit of time is second. Because different device has differnet
 *       hardware granularity function will return actual time in hardware.
 * @param[in] dev_id device id
 * @param[in] time aging time
 * @param[out] time actual aging time
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_age_time_set(dev_id, time);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP host entry aging time on one particular device.
 * @param[in] dev_id device id
 * @param[out] time aging time
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_age_time_get(dev_id, time);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP WCMP hash key mode.
 * @param[in] dev_id device id
 * @param[in] hash_mode hash mode
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_wcmp_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_wcmp_hash_mode_set(dev_id, hash_mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP WCMP hash key mode.
 * @param[in] dev_id device id
 * @param[out] hash_mode hash mode
 * @return SW_OK or error code
 */
sw_error_t
fal_ip_wcmp_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_wcmp_hash_mode_get(dev_id, hash_mode);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_vrf_base_addr_set(a_uint32_t dev_id,
                             a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_vrf_base_addr_set(dev_id, vrf_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_vrf_base_addr_get(a_uint32_t dev_id,
                             a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_vrf_base_addr_get(dev_id, vrf_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_vrf_base_mask_set(a_uint32_t dev_id,
                             a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_vrf_base_mask_set(dev_id, vrf_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_vrf_base_mask_get(a_uint32_t dev_id,
                             a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_vrf_base_mask_get(dev_id, vrf_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_default_route_set(a_uint32_t dev_id,
			a_uint32_t droute_id, fal_default_route_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_default_route_set(dev_id, droute_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_default_route_get(a_uint32_t dev_id,
                             a_uint32_t droute_id, fal_default_route_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_default_route_get(dev_id, droute_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_host_route_set(a_uint32_t dev_id,
                             a_uint32_t hroute_id, fal_host_route_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_host_route_set(dev_id, hroute_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_host_route_get(a_uint32_t dev_id,
                             a_uint32_t hroute_id, fal_host_route_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_host_route_get(dev_id, hroute_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_wcmp_entry_set(a_uint32_t dev_id, a_uint32_t wcmp_id,
    							fal_ip_wcmp_t * wcmp)
{
	sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_wcmp_entry_set(dev_id, wcmp_id, wcmp);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_wcmp_entry_get(a_uint32_t dev_id, a_uint32_t wcmp_id,
    							fal_ip_wcmp_t * wcmp)
{
	sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_wcmp_entry_get(dev_id, wcmp_id, wcmp);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_rfs_ip4_rule_set(a_uint32_t dev_id, fal_ip4_rfs_t * rfs)
{
	sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_rfs_ip4_rule_set(dev_id, rfs);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_rfs_ip6_rule_set(a_uint32_t dev_id, fal_ip6_rfs_t * rfs)
{
	sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_rfs_ip6_rule_set(dev_id, rfs);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_rfs_ip4_rule_del(a_uint32_t dev_id, fal_ip4_rfs_t * rfs)
{
	sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_rfs_ip4_rule_del(dev_id, rfs);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_rfs_ip6_rule_del(a_uint32_t dev_id, fal_ip6_rfs_t * rfs)
{
	sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ip_rfs_ip6_rule_del(dev_id, rfs);
    FAL_API_UNLOCK;
    return rv;
}

int ssdk_rfs_ip4_rule_set(u16 vid, u32 ip, u8* mac, u8 ldb, int is_set)
{
	fal_ip4_rfs_t entry;
	memcpy(&entry.mac_addr, mac, 6);
	entry.ip4_addr = ip;
	entry.load_balance = ldb;
	entry.vid = vid;
	if(is_set)
		return fal_ip_rfs_ip4_rule_set(0, &entry);
	else
		return fal_ip_rfs_ip4_rule_del(0, &entry);
}

int ssdk_rfs_ip6_rule_set(u16 vid, u8* ip, u8* mac, u8 ldb, int is_set)
{
	fal_ip6_rfs_t entry;
	memcpy(&entry.mac_addr, mac, 6);
	memcpy(&entry.ip6_addr, ip, sizeof(entry.ip6_addr));
	entry.load_balance = ldb;
	entry.vid = vid;
	if(is_set)
		return fal_ip_rfs_ip6_rule_set(0, &entry);
	else
		return fal_ip_rfs_ip6_rule_del(0, &entry);
}


#if 0
int
ssdk_ip_rfs_ip4_rule_set(ssdk_ip4_rfs_t * rfs)
{
	fal_ip4_rfs_t entry;
	memcpy(&entry.mac_addr, rfs->mac_addr, 6);
	entry.ip4_addr = rfs->ip4_addr;
	entry.load_balance = rfs->load_balance;
	entry.vid = rfs->vid;
	return fal_ip_rfs_ip4_rule_set(0, &entry);
}

int
ssdk_ip_rfs_ip4_rule_del(ssdk_ip4_rfs_t * rfs)
{
	fal_ip4_rfs_t entry;
	memcpy(&entry.mac_addr, rfs->mac_addr, 6);
	entry.ip4_addr = rfs->ip4_addr;
	entry.load_balance = rfs->load_balance;
	entry.vid = rfs->vid;
	return fal_ip_rfs_ip4_rule_del(0, &entry);
}

int
ssdk_ip_rfs_ip6_rule_set(ssdk_ip6_rfs_t * rfs)
{
	fal_ip6_rfs_t entry;
	memcpy(&entry.mac_addr, rfs->mac_addr, 6);
	memcpy(&entry.ip6_addr, rfs->ip6_addr, sizeof(rfs->ip6_addr));
	entry.load_balance = rfs->load_balance;
	entry.vid = rfs->vid;
	return fal_ip_rfs_ip6_rule_set(0, &entry);
}

int
ssdk_ip_rfs_ip6_rule_del(ssdk_ip6_rfs_t * rfs)
{
	fal_ip6_rfs_t entry;
	memcpy(&entry.mac_addr, rfs->mac_addr, 6);
	memcpy(&entry.ip6_addr, rfs->ip6_addr, sizeof(rfs->ip6_addr));
	entry.load_balance = rfs->load_balance;
	entry.vid = rfs->vid;
	return fal_ip_rfs_ip6_rule_del(0, &entry);
}



EXPORT_SYMBOL(ssdk_ip_rfs_ip4_rule_set);
EXPORT_SYMBOL(ssdk_ip_rfs_ip4_rule_del);
EXPORT_SYMBOL(ssdk_ip_rfs_ip6_rule_set);
EXPORT_SYMBOL(ssdk_ip_rfs_ip6_rule_del);
#endif
/**
 * @brief Set default flow forward command
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] type traffic flow type pass through switch core
 * @param[in] fal_default_flow_cmd_t default flow forward command when flow table mismatch
 * @return SW_OK or error code
 */
sw_error_t
fal_default_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_default_flow_cmd_set(dev_id, vrf_id, type, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get flow type traffic default forward command.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] type traffic flow type pass through switch core
 * @param[out] fal_default_flow_cmd_t default flow forward command when flow table mismatch
 * @return SW_OK or error code
 */
sw_error_t
fal_default_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_default_flow_cmd_get(dev_id, vrf_id, type, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set default route flow forward command
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] type traffic flow type pass through switch core
 * @param[in] fal_default_flow_cmd_t default route flow forward command when flow table mismatch
 * @return SW_OK or error code
 */
sw_error_t
fal_default_rt_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_default_rt_flow_cmd_set(dev_id, vrf_id, type, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get flow type traffic default forward command.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] type traffic flow type pass through switch core
 * @param[out] fal_default_flow_cmd_t default route flow forward command when flow table mismatch
 * @return SW_OK or error code
 */
sw_error_t
fal_default_rt_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_default_rt_flow_cmd_get(dev_id, vrf_id, type, cmd);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_network_route_get(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type,
			fal_network_route_entry_t *entry)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_network_route_get(dev_id, index, type, entry);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_vsi_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_vsi_sg_cfg_get(dev_id, vsi, sg_cfg);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_network_route_del(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_network_route_del(dev_id, index, type);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_port_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_port_sg_cfg_set(dev_id, port_id, sg_cfg);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_port_intf_get(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_port_intf_get(dev_id, port_id, id);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_vsi_arp_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_vsi_arp_sg_cfg_set(dev_id, vsi, arp_sg_cfg);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_pub_addr_get(a_uint32_t dev_id,
			a_uint32_t index, fal_ip_pub_addr_t *entry)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_pub_addr_get(dev_id, index, entry);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_port_intf_set(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_port_intf_set(dev_id, port_id, id);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_vsi_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_vsi_sg_cfg_set(dev_id, vsi, sg_cfg);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_port_macaddr_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_port_macaddr_set(dev_id, port_id, macaddr);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_vsi_intf_get(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_vsi_intf_get(dev_id, vsi, id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_network_route_add(a_uint32_t dev_id,
			a_uint32_t index,
			fal_network_route_entry_t *entry)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_network_route_add(dev_id, index, entry);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_port_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_port_sg_cfg_get(dev_id, port_id, sg_cfg);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_intf_get(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_intf_get(dev_id, index, entry);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_pub_addr_set(a_uint32_t dev_id,
			a_uint32_t index, fal_ip_pub_addr_t *entry)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_pub_addr_set(dev_id, index, entry);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_route_mismatch_action_get(a_uint32_t dev_id, fal_fwd_cmd_t *action)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_route_mismatch_get(dev_id, action);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_vsi_arp_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_vsi_arp_sg_cfg_get(dev_id, vsi, arp_sg_cfg);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_port_arp_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_port_arp_sg_cfg_set(dev_id, port_id, arp_sg_cfg);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_vsi_mc_mode_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_vsi_mc_mode_set(dev_id, vsi, cfg);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_vsi_intf_set(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_vsi_intf_set(dev_id, vsi, id);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_nexthop_get(a_uint32_t dev_id,
			a_uint32_t index, fal_ip_nexthop_t *entry)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_nexthop_get(dev_id, index, entry);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_route_mismatch_action_set(a_uint32_t dev_id, fal_fwd_cmd_t action)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_route_mismatch_set(dev_id, action);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_intf_set(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_intf_set(dev_id, index, entry);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_vsi_mc_mode_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_vsi_mc_mode_get(dev_id, vsi, cfg);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_port_macaddr_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_port_macaddr_get(dev_id, port_id, macaddr);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_port_arp_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_port_arp_sg_cfg_get(dev_id, port_id, arp_sg_cfg);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_ip_nexthop_set(a_uint32_t dev_id,
			a_uint32_t index, fal_ip_nexthop_t *entry)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_ip_nexthop_set(dev_id, index, entry);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_ip_global_ctrl_set(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ip_global_ctrl_set(dev_id, cfg);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_ip_global_ctrl_get(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ip_global_ctrl_get(dev_id, cfg);
	FAL_API_UNLOCK;
	return rv;
}
#endif
/*insert flag for outter fal, don't remove it*/

/**
 * @}
 */
