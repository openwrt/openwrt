/*
 * Copyright (c) 2014, 2015, 2017, The Linux Foundation. All rights reserved.
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
#include "fal_ip.h"
#include "fal_uk_if.h"


sw_error_t
fal_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_HOST_ADD, dev_id, host_entry);
    return rv;
}

sw_error_t
fal_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_HOST_DEL, dev_id, del_mode, host_entry);
    return rv;
}

sw_error_t
fal_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_HOST_GET, dev_id, get_mode, host_entry);
    return rv;
}

sw_error_t
fal_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode, fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_HOST_NEXT, dev_id, next_mode, host_entry);
    return rv;
}

sw_error_t
fal_ip_host_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_HOST_COUNTER_BIND, dev_id, entry_id, cnt_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_ip_host_pppoe_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t pppoe_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_HOST_PPPOE_BIND, dev_id, entry_id, pppoe_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_ip_pt_arp_learn_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t flags)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_PT_ARP_LEARN_SET, dev_id, port_id, flags);
    return rv;
}

sw_error_t
fal_ip_pt_arp_learn_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t * flags)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_PT_ARP_LEARN_GET, dev_id, port_id, flags);
    return rv;
}

sw_error_t
fal_ip_arp_learn_set(a_uint32_t dev_id, fal_arp_learn_mode_t mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_ARP_LEARN_SET, dev_id, (a_uint32_t)mode);
    return rv;
}

sw_error_t
fal_ip_arp_learn_get(a_uint32_t dev_id, fal_arp_learn_mode_t * mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_ARP_LEARN_GET, dev_id, mode);
    return rv;
}

sw_error_t
fal_ip_source_guard_set(a_uint32_t dev_id, fal_port_t port_id, fal_source_guard_mode_t mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_SOURCE_GUARD_SET, dev_id, port_id, (a_uint32_t)mode);
    return rv;
}

sw_error_t
fal_ip_source_guard_get(a_uint32_t dev_id, fal_port_t port_id, fal_source_guard_mode_t * mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_SOURCE_GUARD_GET, dev_id, port_id, mode);
    return rv;
}

sw_error_t
fal_ip_arp_guard_set(a_uint32_t dev_id, fal_port_t port_id, fal_source_guard_mode_t mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_ARP_GUARD_SET, dev_id, port_id, (a_uint32_t)mode);
    return rv;
}

sw_error_t
fal_ip_arp_guard_get(a_uint32_t dev_id, fal_port_t port_id, fal_source_guard_mode_t * mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_ARP_GUARD_GET, dev_id, port_id, mode);
    return rv;
}

sw_error_t
fal_ip_route_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_ROUTE_STATUS_SET, dev_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_ip_route_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_ROUTE_STATUS_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_ip_intf_entry_add(a_uint32_t dev_id, fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_INTF_ENTRY_ADD, dev_id, entry);
    return rv;
}

sw_error_t
fal_ip_intf_entry_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_INTF_ENTRY_DEL, dev_id, del_mode, entry);
    return rv;
}

sw_error_t
fal_ip_intf_entry_next(a_uint32_t dev_id, a_uint32_t next_mode, fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_INTF_ENTRY_NEXT, dev_id, next_mode, entry);
    return rv;
}

sw_error_t
fal_ip_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_UNK_SOURCE_CMD_SET, dev_id, (a_uint32_t) cmd);
    return rv;
}

sw_error_t
fal_ip_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_UNK_SOURCE_CMD_GET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_arp_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_ARP_UNK_SOURCE_CMD_SET, dev_id, (a_uint32_t) cmd);
    return rv;
}

sw_error_t
fal_arp_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_ARP_UNK_SOURCE_CMD_GET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_ip_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_AGE_TIME_SET, dev_id, time);
    return rv;
}

sw_error_t
fal_ip_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_AGE_TIME_GET, dev_id, time);
    return rv;
}

sw_error_t
fal_ip_wcmp_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_WCMP_HASH_MODE_SET, dev_id, hash_mode);
    return rv;
}

sw_error_t
fal_ip_wcmp_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_WCMP_HASH_MODE_GET, dev_id, hash_mode);
    return rv;
}

sw_error_t
fal_ip_vrf_base_addr_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_VRF_BASE_ADDR_SET, dev_id, vrf_id, addr);
    return rv;
}

sw_error_t
fal_ip_vrf_base_addr_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_VRF_BASE_ADDR_GET, dev_id, vrf_id, addr);
    return rv;
}

sw_error_t
fal_ip_vrf_base_mask_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_VRF_BASE_MASK_SET, dev_id, vrf_id, addr);
    return rv;
}

sw_error_t
fal_ip_vrf_base_mask_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_VRF_BASE_MASK_GET, dev_id, vrf_id, addr);
    return rv;
}

sw_error_t
fal_ip_default_route_set(a_uint32_t dev_id, a_uint32_t droute_id,
    			fal_default_route_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_DEFAULT_ROUTE_SET, dev_id, droute_id, entry);
    return rv;
}

sw_error_t
fal_ip_default_route_get(a_uint32_t dev_id, a_uint32_t droute_id,
    			fal_default_route_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_DEFAULT_ROUTE_GET, dev_id, droute_id, entry);
    return rv;
}

sw_error_t
fal_ip_host_route_set(a_uint32_t dev_id, a_uint32_t hroute_id,
    			fal_host_route_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_HOST_ROUTE_SET, dev_id, hroute_id, entry);
    return rv;
}

sw_error_t
fal_ip_host_route_get(a_uint32_t dev_id, a_uint32_t hroute_id,
    			fal_host_route_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_IP_HOST_ROUTE_GET, dev_id, hroute_id, entry);
    return rv;
}

sw_error_t
fal_ip_wcmp_entry_set(a_uint32_t dev_id, a_uint32_t wcmp_id,
    							fal_ip_wcmp_t * wcmp)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_WCMP_ENTRY_SET, dev_id, wcmp_id, wcmp);
	return rv;
}


sw_error_t
fal_ip_wcmp_entry_get(a_uint32_t dev_id, a_uint32_t wcmp_id,
    							fal_ip_wcmp_t * wcmp)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_WCMP_ENTRY_GET, dev_id, wcmp_id, wcmp);
	return rv;
}

sw_error_t
fal_ip_rfs_ip4_rule_set(a_uint32_t dev_id,
	fal_ip4_rfs_t * rfs)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_RFS_IP4_SET, dev_id, rfs);
	return rv;
}

sw_error_t
fal_ip_rfs_ip6_rule_set(a_uint32_t dev_id,
	fal_ip6_rfs_t * rfs)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_RFS_IP6_SET, dev_id, rfs);
	return rv;
}

sw_error_t
fal_ip_rfs_ip4_rule_del(a_uint32_t dev_id,
	fal_ip4_rfs_t * rfs)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_RFS_IP4_DEL, dev_id, rfs);
	return rv;
}

sw_error_t
fal_ip_rfs_ip6_rule_del(a_uint32_t dev_id,
	fal_ip6_rfs_t * rfs)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_RFS_IP6_DEL, dev_id, rfs);
	return rv;
}

sw_error_t
fal_default_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_DEFAULT_FLOW_CMD_SET, dev_id, vrf_id, type, (a_uint32_t) cmd);
	return rv;
}

sw_error_t
fal_default_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_DEFAULT_FLOW_CMD_GET, dev_id, vrf_id, type, cmd);
	return rv;
}

sw_error_t
fal_default_rt_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_DEFAULT_RT_FLOW_CMD_SET, dev_id, vrf_id, type, (a_uint32_t) cmd);
	return rv;
}

sw_error_t
fal_default_rt_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_DEFAULT_RT_FLOW_CMD_GET, dev_id, vrf_id, type, cmd);
	return rv;
}

sw_error_t
fal_ip_vsi_arp_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_VIS_ARP_SG_CFG_SET, dev_id, vsi, arp_sg_cfg);
	return rv;
}

sw_error_t
fal_ip_vsi_arp_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_VIS_ARP_SG_CFG_GET, dev_id, vsi, arp_sg_cfg);
	return rv;
}

sw_error_t
fal_ip_network_route_add(a_uint32_t dev_id,
			a_uint32_t index,
			fal_network_route_entry_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_NETWORK_ROUTE_ADD, dev_id, index, entry);
	return rv;
}

sw_error_t
fal_ip_network_route_get(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type,
			fal_network_route_entry_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_NETWORK_ROUTE_GET, dev_id, index, type, entry);
	return rv;
}

sw_error_t
fal_ip_intf_set(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_INTF_SET, dev_id, index, entry);
	return rv;
}

sw_error_t
fal_ip_intf_get(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_INTF_GET, dev_id, index, entry);
	return rv;
}

sw_error_t
fal_ip_vsi_intf_set(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_VSI_INTF_SET, dev_id, vsi, id);
	return rv;
}

sw_error_t
fal_ip_vsi_intf_get(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_VSI_INTF_GET, dev_id, vsi, id);
	return rv;
}

sw_error_t
fal_ip_port_intf_set(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PORT_INTF_SET, dev_id, port_id, id);
	return rv;
}

sw_error_t
fal_ip_port_intf_get(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PORT_INTF_GET, dev_id, port_id, id);
	return rv;
}

sw_error_t
fal_ip_nexthop_set(a_uint32_t dev_id, a_uint32_t index,
    			fal_ip_nexthop_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_NEXTHOP_SET, dev_id, index, entry);
	return rv;
}

sw_error_t
fal_ip_nexthop_get(a_uint32_t dev_id, a_uint32_t index,
    			fal_ip_nexthop_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_NEXTHOP_GET, dev_id, index, entry);
	return rv;
}

sw_error_t
fal_ip_port_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PORT_SG_SET, dev_id, port_id, sg_cfg);
	return rv;
}

sw_error_t
fal_ip_port_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PORT_SG_GET, dev_id, port_id, sg_cfg);
	return rv;
}

sw_error_t
fal_ip_vsi_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_VSI_SG_GET, dev_id, vsi, sg_cfg);
	return rv;
}

sw_error_t
fal_ip_vsi_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_VSI_SG_SET, dev_id, vsi, sg_cfg);
	return rv;
}

sw_error_t
fal_ip_pub_addr_set(a_uint32_t dev_id, a_uint32_t index,
			fal_ip_pub_addr_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PUB_IP_SET, dev_id, index, entry);
	return rv;
}

sw_error_t
fal_ip_network_route_del(a_uint32_t dev_id, a_uint32_t index, a_uint8_t type)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_NETWORK_ROUTE_DEL, dev_id, index, type);
	return rv;
}

sw_error_t
fal_ip_pub_addr_get(a_uint32_t dev_id, a_uint32_t index, fal_ip_pub_addr_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PUB_IP_GET, dev_id, index, entry);
	return rv;
}

sw_error_t
    fal_ip_port_macaddr_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PORT_MAC_SET, dev_id, port_id, macaddr);
	return rv;
}

sw_error_t
fal_ip_port_macaddr_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PORT_MAC_GET, dev_id, port_id, macaddr);
	return rv;
}

sw_error_t
fal_ip_route_mismatch_action_set(a_uint32_t dev_id, fal_fwd_cmd_t action)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_ROUTE_MISS_SET, dev_id, (a_uint32_t)action);
	return rv;
}

sw_error_t
fal_ip_route_mismatch_action_get(a_uint32_t dev_id, fal_fwd_cmd_t *action)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_ROUTE_MISS_GET, dev_id, action);
	return rv;
}

sw_error_t
fal_ip_port_arp_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PORT_ARP_SG_SET, dev_id, port_id, arp_sg_cfg);
	return rv;
}

sw_error_t
fal_ip_port_arp_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_PORT_ARP_SG_GET, dev_id, port_id, arp_sg_cfg);
	return rv;
}

sw_error_t
fal_ip_vsi_mc_mode_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_VSI_MC_MODE_SET, dev_id, vsi, cfg);
	return rv;
}

sw_error_t
fal_ip_vsi_mc_mode_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_IP_VSI_MC_MODE_GET, dev_id, vsi, cfg);
	return rv;
}

sw_error_t
fal_ip_global_ctrl_get(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_GLOBAL_CTRL_GET, dev_id, cfg);
	return rv;
}

sw_error_t
fal_ip_global_ctrl_set(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_GLOBAL_CTRL_SET, dev_id, cfg);
	return rv;
}

