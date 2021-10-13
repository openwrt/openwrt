/*
 * Copyright (c) 2016-2017, 2019, The Linux Foundation. All rights reserved.
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
 * @defgroup
 * @{
 */
#include "sw.h"
#include "fal_flow.h"
#include "hppe_ip_reg.h"
#include "hppe_ip.h"
#include "hppe_flow_reg.h"
#include "hppe_flow.h"
#include "adpt_hppe.h"
#include "adpt.h"

#if defined(CPPE)
#include "adpt_cppe_flow.h"
#endif


#define FLOW_ENTRY_TYPE_IPV4 0
#define FLOW_ENTRY_TYPE_IPV6 1
#define FLOW_TUPLE_TYPE_3    0

#ifndef IN_FLOW_MINI
sw_error_t
adpt_hppe_ip_flow_host_data_rd_add(a_uint32_t dev_id, fal_host_entry_t * host_entry)

{
	a_uint8_t mode = 0, type = 0;
	sw_error_t rv = SW_OK;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(host_entry);

	mode = host_entry->flags >> 24;
	type = host_entry->flags & 0xff;

	if ((type & FAL_IP_IP4_ADDR) == FAL_IP_IP4_ADDR) {
		union host_tbl_u entry;
		entry.bf.valid= host_entry->status;
		entry.bf.key_type = 0;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->port_id;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ip_addr = host_entry->ip4_addr;
		rv = hppe_flow_host_ipv4_data_rd_add(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	} else if ((type & FAL_IP_IP6_ADDR) == FAL_IP_IP6_ADDR) {
		union host_ipv6_tbl_u entry;
		entry.bf.valid= host_entry->status;
		entry.bf.key_type = 2;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->port_id;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ipv6_addr_0 = host_entry->ip6_addr.ul[3];
		entry.bf.ipv6_addr_1 = host_entry->ip6_addr.ul[3] >> 10 | \
							host_entry->ip6_addr.ul[2] << 22;
		entry.bf.ipv6_addr_2 = host_entry->ip6_addr.ul[2] >> 10 | \
							host_entry->ip6_addr.ul[1] << 22;
		entry.bf.ipv6_addr_3 = host_entry->ip6_addr.ul[1] >> 10 | \
							host_entry->ip6_addr.ul[0] << 22;
		entry.bf.ipv6_addr_4 = host_entry->ip6_addr.ul[0] >> 10;
		rv = hppe_flow_host_ipv6_data_rd_add(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	}

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_flow_host_data_add(a_uint32_t dev_id, fal_host_entry_t * host_entry)
{
	a_uint8_t mode = 0, type = 0;
	sw_error_t rv = SW_OK;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(host_entry);

	mode = host_entry->flags >> 24;
	type = host_entry->flags & 0xff;

	if ((type & FAL_IP_IP4_ADDR) == FAL_IP_IP4_ADDR) {
		union host_tbl_u entry;
		entry.bf.valid= host_entry->status;
		entry.bf.key_type = 0;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->port_id;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ip_addr = host_entry->ip4_addr;
		rv = hppe_flow_host_ipv4_data_add(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	} else if ((type & FAL_IP_IP6_ADDR) == FAL_IP_IP6_ADDR) {
		union host_ipv6_tbl_u entry;
		entry.bf.valid= host_entry->status;
		entry.bf.key_type = 2;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->port_id;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ipv6_addr_0 = host_entry->ip6_addr.ul[3];
		entry.bf.ipv6_addr_1 = host_entry->ip6_addr.ul[3] >> 10 | \
							host_entry->ip6_addr.ul[2] << 22;
		entry.bf.ipv6_addr_2 = host_entry->ip6_addr.ul[2] >> 10 | \
							host_entry->ip6_addr.ul[1] << 22;
		entry.bf.ipv6_addr_3 = host_entry->ip6_addr.ul[1] >> 10 | \
							host_entry->ip6_addr.ul[0] << 22;
		entry.bf.ipv6_addr_4 = host_entry->ip6_addr.ul[0] >> 10;
		rv = hppe_flow_host_ipv6_data_add(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	}

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_flow_host_data_get(a_uint32_t dev_id, a_uint32_t get_mode,
                    fal_host_entry_t *host_entry)
{
	a_uint8_t mode = 0, type = 0;
	sw_error_t rv = SW_OK;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(host_entry);

	mode = host_entry->flags >> 24;
	type = host_entry->flags & 0xff;

	if (get_mode & FAL_IP_ENTRY_ID_EN)
		mode = 1;
	else if (get_mode & FAL_IP_ENTRY_IPADDR_EN)
		mode  = 0;

	if ((type & FAL_IP_IP4_ADDR) == FAL_IP_IP4_ADDR) {
		union host_tbl_u entry;
		entry.bf.key_type = 0;
		entry.bf.ip_addr = host_entry->ip4_addr;
		rv = hppe_flow_host_ipv4_data_get(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
		host_entry->ip4_addr = entry.bf.ip_addr;
		host_entry->lan_wan = entry.bf.lan_wan;
		host_entry->port_id = entry.bf.dst_info;
		host_entry->syn_toggle = entry.bf.syn_toggle;
		host_entry->action = entry.bf.fwd_cmd;
		host_entry->status = entry.bf.valid;
	} else if ((type & FAL_IP_IP6_ADDR) == FAL_IP_IP6_ADDR) {
		union host_ipv6_tbl_u entry;
		entry.bf.key_type = 2;
		entry.bf.ipv6_addr_0 = host_entry->ip6_addr.ul[3];
		entry.bf.ipv6_addr_1 = host_entry->ip6_addr.ul[3] >> 10 | \
							host_entry->ip6_addr.ul[2] << 22;
		entry.bf.ipv6_addr_2 = host_entry->ip6_addr.ul[2] >> 10 | \
							host_entry->ip6_addr.ul[1] << 22;
		entry.bf.ipv6_addr_3 = host_entry->ip6_addr.ul[1] >> 10 | \
							host_entry->ip6_addr.ul[0] << 22;
		entry.bf.ipv6_addr_4 = host_entry->ip6_addr.ul[0] >> 10;
		rv = hppe_flow_host_ipv6_data_get(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
		host_entry->ip6_addr.ul[3] = entry.bf.ipv6_addr_0 | entry.bf.ipv6_addr_1 << 10;
		host_entry->ip6_addr.ul[2] = entry.bf.ipv6_addr_1 >> 22 | entry.bf.ipv6_addr_2 << 10;
		host_entry->ip6_addr.ul[1] = entry.bf.ipv6_addr_2 >> 22 | entry.bf.ipv6_addr_3 << 10;
		host_entry->ip6_addr.ul[0] = entry.bf.ipv6_addr_3 >> 22 | entry.bf.ipv6_addr_4 << 10;
		host_entry->lan_wan = entry.bf.lan_wan;
		host_entry->port_id = entry.bf.dst_info;
		host_entry->syn_toggle = entry.bf.syn_toggle;
		host_entry->action = entry.bf.fwd_cmd;
		host_entry->status = entry.bf.valid;
	}
	return rv;
}

sw_error_t
adpt_hppe_ip_flow_host_data_del(a_uint32_t dev_id, a_uint32_t del_mode,
                    fal_host_entry_t * host_entry)
{
	a_uint8_t mode = 0, type = 0;
	sw_error_t rv = SW_OK;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(host_entry);

	mode = host_entry->flags >> 24;
	type = host_entry->flags & 0xff;

	if (del_mode & FAL_IP_ENTRY_ID_EN)
		mode = 1;
	else if (del_mode & FAL_IP_ENTRY_IPADDR_EN)
		mode  = 0;
	else if (del_mode & FAL_IP_ENTRY_ALL_EN) {
		return hppe_flow_host_flush_common(dev_id);
	}

	if ((type & FAL_IP_IP4_ADDR) == FAL_IP_IP4_ADDR) {
		union host_tbl_u entry;
		entry.bf.valid= 1;
		entry.bf.key_type = 0;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->port_id;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ip_addr = host_entry->ip4_addr;
		rv = hppe_flow_host_ipv4_data_del(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	} else if ((type & FAL_IP_IP6_ADDR) == FAL_IP_IP6_ADDR) {
		union host_ipv6_tbl_u entry;
		entry.bf.valid= 1;
		entry.bf.key_type = 2;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->port_id;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ipv6_addr_0 = host_entry->ip6_addr.ul[3];
		entry.bf.ipv6_addr_1 = host_entry->ip6_addr.ul[3] >> 10 | \
							host_entry->ip6_addr.ul[2] << 22;
		entry.bf.ipv6_addr_2 = host_entry->ip6_addr.ul[2] >> 10 | \
							host_entry->ip6_addr.ul[1] << 22;
		entry.bf.ipv6_addr_3 = host_entry->ip6_addr.ul[1] >> 10 | \
							host_entry->ip6_addr.ul[0] << 22;
		entry.bf.ipv6_addr_4 = host_entry->ip6_addr.ul[0] >> 10;
		rv = hppe_flow_host_ipv6_data_del(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	}
	return rv;
}


sw_error_t
adpt_hppe_flow_entry_host_op_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode, /*index or hash*/
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;
	a_uint32_t type = 0;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_entry);

	type = flow_entry->entry_type;
	if ((type & FAL_FLOW_IP4_5TUPLE_ADDR) == FAL_FLOW_IP4_5TUPLE_ADDR) {
		union in_flow_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
			entry.bf0.l4_port1 = flow_entry->snat_srcport;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
			entry.bf3.l4_port2 = flow_entry->dnat_dstport;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_entry_host_op_ipv4_5tuple_add(dev_id, (a_uint32_t)add_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP6_5TUPLE_ADDR) == FAL_FLOW_IP6_5TUPLE_ADDR) {
		union in_flow_ipv6_5tuple_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_entry_host_op_ipv6_5tuple_add(dev_id, (a_uint32_t)add_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP4_3TUPLE_ADDR) == FAL_FLOW_IP4_3TUPLE_ADDR) {
		union in_flow_3tuple_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_entry_host_op_ipv4_3tuple_add(dev_id, (a_uint32_t)add_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP6_3TUPLE_ADDR) == FAL_FLOW_IP6_3TUPLE_ADDR) {
		union in_flow_ipv6_3tuple_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf1.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf0.port_vp2 = flow_entry->bridge_port;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_entry_host_op_ipv6_3tuple_add(dev_id, (a_uint32_t)add_mode, &flow_entry->entry_id, &entry);
	} else
		return SW_FAIL;
	if (rv == SW_OK) {
		union eg_flow_tree_map_tbl_u eg_treemap;
		eg_treemap.bf.tree_id = flow_entry->tree_id;
		rv = hppe_eg_flow_tree_map_tbl_set(dev_id, flow_entry->entry_id, &eg_treemap);
	}
	return rv;
}

sw_error_t
adpt_hppe_flow_entry_host_op_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;
	a_uint32_t type = 0;
	a_uint32_t entry_id = 0;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_entry);

	type = flow_entry->entry_type;
	if ((type & FAL_FLOW_IP4_5TUPLE_ADDR) == FAL_FLOW_IP4_5TUPLE_ADDR) {
		union in_flow_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_entry_host_op_ipv4_5tuple_get(dev_id, get_mode, &entry_id, &entry);
		flow_entry->entry_id = entry_id;
		flow_entry->host_addr_type = entry.bf0.host_addr_index_type;
		flow_entry->host_addr_index = entry.bf0.host_addr_index;
		flow_entry->protocol = entry.bf0.protocol_type;
		flow_entry->age = entry.bf0.age;
		flow_entry->src_intf_valid = entry.bf0.src_l3_if_valid;
		flow_entry->src_intf_index = entry.bf0.src_l3_if;
		flow_entry->fwd_type = entry.bf0.fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			flow_entry->snat_nexthop = entry.bf0.next_hop1;
			flow_entry->snat_srcport = entry.bf0.l4_port1;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			flow_entry->dnat_nexthop = entry.bf3.next_hop2;
			flow_entry->dnat_dstport = entry.bf3.l4_port2;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			flow_entry->route_nexthop = entry.bf2.next_hop3;
			flow_entry->port_valid = entry.bf2.port_vp_valid1;
			flow_entry->route_port = entry.bf2.port_vp1;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			flow_entry->bridge_port = entry.bf1.port_vp2;
		}
		flow_entry->deacclr_en = entry.bf0.de_acce;
		flow_entry->copy_tocpu_en = entry.bf0.copy_to_cpu_en;
		flow_entry->syn_toggle = entry.bf0.syn_toggle;
		flow_entry->pri_profile = entry.bf0.pri_profile_0 |\
								entry.bf0.pri_profile_1 << 1;
		flow_entry->sevice_code = entry.bf0.service_code;
		flow_entry->flow_ip.ipv4 = entry.bf0.ip_addr_0 |\
					   entry.bf0.ip_addr_1 << 20;
		flow_entry->src_port = entry.bf0.l4_sport;
		flow_entry->dst_port = entry.bf0.l4_dport_0 |\
					entry.bf0.l4_dport_1 << 4;
	} else if ((type & FAL_FLOW_IP6_5TUPLE_ADDR) == FAL_FLOW_IP6_5TUPLE_ADDR) {
		union in_flow_ipv6_5tuple_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_entry_host_op_ipv6_5tuple_get(dev_id, get_mode, &entry_id, &entry);
		flow_entry->entry_id = entry_id;
		flow_entry->host_addr_type = entry.bf0.host_addr_index_type;
		flow_entry->host_addr_index = entry.bf0.host_addr_index;
		flow_entry->protocol = entry.bf0.protocol_type;
		flow_entry->age = entry.bf0.age;
		flow_entry->src_intf_valid = entry.bf0.src_l3_if_valid;
		flow_entry->src_intf_index = entry.bf0.src_l3_if;
		flow_entry->fwd_type = entry.bf0.fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			flow_entry->snat_nexthop = entry.bf0.next_hop1;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			flow_entry->dnat_nexthop = entry.bf3.next_hop2;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			flow_entry->route_nexthop = entry.bf2.next_hop3;
			flow_entry->port_valid = entry.bf2.port_vp_valid1;
			flow_entry->route_port = entry.bf2.port_vp1;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			flow_entry->bridge_port = entry.bf1.port_vp2;
		}
		flow_entry->deacclr_en = entry.bf0.de_acce;
		flow_entry->copy_tocpu_en = entry.bf0.copy_to_cpu_en;
		flow_entry->syn_toggle = entry.bf0.syn_toggle;
		flow_entry->pri_profile = entry.bf0.pri_profile_0 |\
								entry.bf0.pri_profile_1 << 1;
		flow_entry->sevice_code = entry.bf0.service_code;
		flow_entry->flow_ip.ipv6.ul[3] = entry.bf0.ip_addr_0 |\
					   entry.bf0.ip_addr_1 << 20;
		flow_entry->flow_ip.ipv6.ul[2] = entry.bf0.ip_addr_1 >> 12 |\
					   entry.bf0.ip_addr_2 << 20;
		flow_entry->flow_ip.ipv6.ul[1] = entry.bf0.ip_addr_2 >> 12 |\
					   entry.bf0.ip_addr_3 << 20;
		flow_entry->flow_ip.ipv6.ul[0] = entry.bf0.ip_addr_3 >> 12 |\
					   entry.bf0.ip_addr_4 << 20;
		flow_entry->src_port = entry.bf0.l4_sport;
		flow_entry->dst_port = entry.bf0.l4_dport_0 |\
					entry.bf0.l4_dport_1 << 4;
	} else if ((type & FAL_FLOW_IP4_3TUPLE_ADDR) == FAL_FLOW_IP4_3TUPLE_ADDR) {
		union in_flow_3tuple_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_entry_host_op_ipv4_3tuple_get(dev_id, get_mode, &entry_id, &entry);
		flow_entry->entry_id = entry_id;
		flow_entry->host_addr_type = entry.bf0.host_addr_index_type;
		flow_entry->host_addr_index = entry.bf0.host_addr_index;
		flow_entry->protocol = entry.bf0.protocol_type;
		flow_entry->age = entry.bf0.age;
		flow_entry->src_intf_valid = entry.bf0.src_l3_if_valid;
		flow_entry->src_intf_index = entry.bf0.src_l3_if;
		flow_entry->fwd_type = entry.bf0.fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			flow_entry->snat_nexthop = entry.bf0.next_hop1;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			flow_entry->dnat_nexthop = entry.bf3.next_hop2;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			flow_entry->route_nexthop = entry.bf2.next_hop3;
			flow_entry->port_valid = entry.bf2.port_vp_valid1;
			flow_entry->route_port = entry.bf2.port_vp1;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			flow_entry->bridge_port = entry.bf1.port_vp2;
		}
		flow_entry->deacclr_en = entry.bf0.de_acce;
		flow_entry->copy_tocpu_en = entry.bf0.copy_to_cpu_en;
		flow_entry->syn_toggle = entry.bf0.syn_toggle;
		flow_entry->pri_profile = entry.bf0.pri_profile_0 |\
								entry.bf0.pri_profile_1 << 1;
		flow_entry->sevice_code = entry.bf0.service_code;
		flow_entry->flow_ip.ipv4 = entry.bf0.ip_addr_0 |\
					   entry.bf0.ip_addr_1 << 20;
		flow_entry->ip_type = entry.bf0.ip_protocol;
	} else if ((type & FAL_FLOW_IP6_3TUPLE_ADDR) == FAL_FLOW_IP6_3TUPLE_ADDR) {
		union in_flow_ipv6_3tuple_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_entry_host_op_ipv6_3tuple_get(dev_id, get_mode, &entry_id, &entry);
		flow_entry->entry_id = entry_id;
		flow_entry->host_addr_type = entry.bf0.host_addr_index_type;
		flow_entry->host_addr_index = entry.bf0.host_addr_index;
		flow_entry->protocol = entry.bf0.protocol_type;
		flow_entry->age = entry.bf0.age;
		flow_entry->src_intf_valid = entry.bf0.src_l3_if_valid;
		flow_entry->src_intf_index = entry.bf0.src_l3_if;
		flow_entry->fwd_type = entry.bf0.fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			flow_entry->snat_nexthop = entry.bf1.next_hop1;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			flow_entry->dnat_nexthop = entry.bf3.next_hop2;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			flow_entry->route_nexthop = entry.bf2.next_hop3;
			flow_entry->port_valid = entry.bf2.port_vp_valid1;
			flow_entry->route_port = entry.bf2.port_vp1;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			flow_entry->bridge_port = entry.bf0.port_vp2;
		}
		flow_entry->deacclr_en = entry.bf0.de_acce;
		flow_entry->copy_tocpu_en = entry.bf0.copy_to_cpu_en;
		flow_entry->syn_toggle = entry.bf0.syn_toggle;
		flow_entry->pri_profile = entry.bf0.pri_profile_0 |\
								entry.bf0.pri_profile_1 << 1;
		flow_entry->sevice_code = entry.bf0.service_code;
		flow_entry->flow_ip.ipv6.ul[3] = entry.bf0.ip_addr_0 |\
					   entry.bf0.ip_addr_1 << 20;
		flow_entry->flow_ip.ipv6.ul[2] = entry.bf0.ip_addr_1 >> 12 |\
					   entry.bf0.ip_addr_2 << 20;
		flow_entry->flow_ip.ipv6.ul[1] = entry.bf0.ip_addr_2 >> 12 |\
					   entry.bf0.ip_addr_3 << 20;
		flow_entry->flow_ip.ipv6.ul[0] = entry.bf0.ip_addr_3 >> 12 |\
					   entry.bf0.ip_addr_4 << 20;
		flow_entry->ip_type = entry.bf0.ip_protocol;
	} else
		return SW_FAIL;

	if (rv == SW_OK) {
		union eg_flow_tree_map_tbl_u eg_treemap;
		union in_flow_cnt_tbl_u cnt;
		rv = hppe_eg_flow_tree_map_tbl_get(dev_id, flow_entry->entry_id, &eg_treemap);
		flow_entry->tree_id = eg_treemap.bf.tree_id;
		rv = hppe_in_flow_cnt_tbl_get(dev_id, flow_entry->entry_id, &cnt);
		flow_entry->pkt_counter = cnt.bf.hit_pkt_counter;
		flow_entry->byte_counter = cnt.bf.hit_byte_counter_0 | \
					((a_uint64_t)cnt.bf.hit_byte_counter_1 << 32);
	}
	return rv;
}

sw_error_t
adpt_hppe_flow_entry_host_op_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;
	a_uint32_t type = 0;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_entry);

	if (del_mode == FAL_FLOW_OP_MODE_FLUSH)
		return hppe_flow_host_flush_common(dev_id);
		//return hppe_flow_flush_common(dev_id);

	type = flow_entry->entry_type;
	if ((type & FAL_FLOW_IP4_5TUPLE_ADDR) == FAL_FLOW_IP4_5TUPLE_ADDR) {
		union in_flow_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
			entry.bf0.l4_port1 = flow_entry->snat_srcport;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
			entry.bf3.l4_port2 = flow_entry->dnat_dstport;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_entry_host_op_ipv4_5tuple_del(dev_id, del_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP6_5TUPLE_ADDR) == FAL_FLOW_IP6_5TUPLE_ADDR) {
		union in_flow_ipv6_5tuple_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_entry_host_op_ipv6_5tuple_del(dev_id, del_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP4_3TUPLE_ADDR) == FAL_FLOW_IP4_3TUPLE_ADDR) {
		union in_flow_3tuple_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_entry_host_op_ipv4_3tuple_del(dev_id, del_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP6_3TUPLE_ADDR) == FAL_FLOW_IP6_3TUPLE_ADDR) {
		union in_flow_ipv6_3tuple_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf1.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf0.port_vp2 = flow_entry->bridge_port;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_entry_host_op_ipv6_3tuple_del(dev_id, del_mode, &flow_entry->entry_id, &entry);
	} else
		return SW_FAIL;
	return rv;
}

sw_error_t
adpt_hppe_flow_host_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode,
		fal_flow_host_entry_t *flow_host)
{
	sw_error_t rv = SW_OK;
	fal_flow_entry_t *flow_entry = &(flow_host->flow_entry);
	fal_host_entry_t *host_entry = &(flow_host->host_entry);
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_host);

	rv = adpt_hppe_ip_flow_host_data_add(dev_id, host_entry);
	SW_RTN_ON_ERROR(rv);
	rv = adpt_hppe_flow_entry_host_op_add(dev_id, add_mode, flow_entry);
	SW_RTN_ON_ERROR(rv);

	rv = hppe_flow_host_tbl_op_rslt_host_entry_index_get(dev_id, &host_entry->entry_id);
	return rv;
}

sw_error_t
adpt_hppe_flow_host_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_host_entry_t *flow_host)
{
	sw_error_t rv = SW_OK;
	fal_flow_entry_t *flow_entry = &(flow_host->flow_entry);
	fal_host_entry_t *host_entry = &(flow_host->host_entry);
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_host);

	rv = adpt_hppe_ip_flow_host_data_rd_add(dev_id, host_entry);
	rv = adpt_hppe_flow_entry_host_op_get(dev_id, get_mode, flow_entry);
	if(rv != SW_OK)
		return rv;

	rv = hppe_flow_host_tbl_rd_op_rslt_host_entry_index_get(dev_id, &host_entry->entry_id);
	if(rv != SW_OK)
		return rv;

	rv = adpt_hppe_ip_flow_host_data_get(dev_id, get_mode, host_entry);

	return rv;
}

sw_error_t
adpt_hppe_flow_host_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_host_entry_t *flow_host)
{
	fal_flow_entry_t *flow_entry = &(flow_host->flow_entry);
	fal_host_entry_t *host_entry = &(flow_host->host_entry);
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_host);

	adpt_hppe_ip_flow_host_data_del(dev_id, del_mode, host_entry);
	adpt_hppe_flow_entry_host_op_del(dev_id, del_mode, flow_entry);
	return SW_OK;
}


sw_error_t
adpt_hppe_flow_entry_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;
	a_uint32_t type = 0;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_entry);


	type = flow_entry->entry_type;
	if ((type & FAL_FLOW_IP4_5TUPLE_ADDR) == FAL_FLOW_IP4_5TUPLE_ADDR) {
		union in_flow_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_ipv4_5tuple_get(dev_id, get_mode, &flow_entry->entry_id, &entry);
		if (entry.bf0.entry_type != FLOW_ENTRY_TYPE_IPV4 ||
				entry.bf0.protocol_type == FLOW_TUPLE_TYPE_3) {
			return SW_BAD_VALUE;
		}
		flow_entry->host_addr_type = entry.bf0.host_addr_index_type;
		flow_entry->host_addr_index = entry.bf0.host_addr_index;
		flow_entry->protocol = entry.bf0.protocol_type;
		flow_entry->age = entry.bf0.age;
		flow_entry->src_intf_valid = entry.bf0.src_l3_if_valid;
		flow_entry->src_intf_index = entry.bf0.src_l3_if;
		flow_entry->fwd_type = entry.bf0.fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			flow_entry->snat_nexthop = entry.bf0.next_hop1;
			flow_entry->snat_srcport = entry.bf0.l4_port1;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			flow_entry->dnat_nexthop = entry.bf3.next_hop2;
			flow_entry->dnat_dstport = entry.bf3.l4_port2;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			flow_entry->route_nexthop = entry.bf2.next_hop3;
			flow_entry->port_valid = entry.bf2.port_vp_valid1;
			flow_entry->route_port = entry.bf2.port_vp1;
			if (entry.bf2.port_vp1 >= 64)
				flow_entry->route_port |= 0x1000000;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			flow_entry->bridge_port = entry.bf1.port_vp2;
			if (entry.bf1.port_vp2 >= 64)
				flow_entry->bridge_port |= 0x1000000;
		}
		flow_entry->deacclr_en = entry.bf0.de_acce;
		flow_entry->copy_tocpu_en = entry.bf0.copy_to_cpu_en;
		flow_entry->syn_toggle = entry.bf0.syn_toggle;
		flow_entry->pri_profile = entry.bf0.pri_profile_0 |\
								entry.bf0.pri_profile_1 << 1;
		flow_entry->sevice_code = entry.bf0.service_code;
		flow_entry->flow_ip.ipv4 = entry.bf0.ip_addr_0 |\
					   entry.bf0.ip_addr_1 << 20;
		flow_entry->src_port = entry.bf0.l4_sport;
		flow_entry->dst_port = entry.bf0.l4_dport_0 |\
					entry.bf0.l4_dport_1 << 4;
		
	} else if ((type & FAL_FLOW_IP6_5TUPLE_ADDR) == FAL_FLOW_IP6_5TUPLE_ADDR) {
		union in_flow_ipv6_5tuple_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_ipv6_5tuple_get(dev_id, get_mode, &flow_entry->entry_id, &entry);
		if (entry.bf0.entry_type != FLOW_ENTRY_TYPE_IPV6 ||
				entry.bf0.protocol_type == FLOW_TUPLE_TYPE_3) {
			return SW_BAD_VALUE;
		}
		flow_entry->host_addr_type = entry.bf0.host_addr_index_type;
		flow_entry->host_addr_index = entry.bf0.host_addr_index;
		flow_entry->protocol = entry.bf0.protocol_type;
		flow_entry->age = entry.bf0.age;
		flow_entry->src_intf_valid = entry.bf0.src_l3_if_valid;
		flow_entry->src_intf_index = entry.bf0.src_l3_if;
		flow_entry->fwd_type = entry.bf0.fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			flow_entry->snat_nexthop = entry.bf0.next_hop1;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			flow_entry->dnat_nexthop = entry.bf3.next_hop2;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			flow_entry->route_nexthop = entry.bf2.next_hop3;
			flow_entry->port_valid = entry.bf2.port_vp_valid1;
			flow_entry->route_port = entry.bf2.port_vp1;
			if (entry.bf2.port_vp1 >= 64)
				flow_entry->route_port |= 0x1000000;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			flow_entry->bridge_port = entry.bf1.port_vp2;
			if (entry.bf1.port_vp2 >= 64)
				flow_entry->bridge_port |= 0x1000000;
		}
		flow_entry->deacclr_en = entry.bf0.de_acce;
		flow_entry->copy_tocpu_en = entry.bf0.copy_to_cpu_en;
		flow_entry->syn_toggle = entry.bf0.syn_toggle;
		flow_entry->pri_profile = entry.bf0.pri_profile_0 |\
								entry.bf0.pri_profile_1 << 1;
		flow_entry->sevice_code = entry.bf0.service_code;
		flow_entry->flow_ip.ipv6.ul[3] = entry.bf0.ip_addr_0 |\
					   entry.bf0.ip_addr_1 << 20;
		flow_entry->flow_ip.ipv6.ul[2] = entry.bf0.ip_addr_1 >> 12 |\
					   entry.bf0.ip_addr_2 << 20;
		flow_entry->flow_ip.ipv6.ul[1] = entry.bf0.ip_addr_2 >> 12 |\
					   entry.bf0.ip_addr_3 << 20;
		flow_entry->flow_ip.ipv6.ul[0] = entry.bf0.ip_addr_3 >> 12 |\
					   entry.bf0.ip_addr_4 << 20;
		flow_entry->src_port = entry.bf0.l4_sport;
		flow_entry->dst_port = entry.bf0.l4_dport_0 |\
					entry.bf0.l4_dport_1 << 4;
		
	} else if ((type & FAL_FLOW_IP4_3TUPLE_ADDR) == FAL_FLOW_IP4_3TUPLE_ADDR) {
		union in_flow_3tuple_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_ipv4_3tuple_get(dev_id, get_mode, &flow_entry->entry_id, &entry);
		if (entry.bf0.entry_type != FLOW_ENTRY_TYPE_IPV4 ||
				entry.bf0.protocol_type != FLOW_TUPLE_TYPE_3) {
			return SW_BAD_VALUE;
		}
		flow_entry->host_addr_type = entry.bf0.host_addr_index_type;
		flow_entry->host_addr_index = entry.bf0.host_addr_index;
		flow_entry->protocol = entry.bf0.protocol_type;
		flow_entry->age = entry.bf0.age;
		flow_entry->src_intf_valid = entry.bf0.src_l3_if_valid;
		flow_entry->src_intf_index = entry.bf0.src_l3_if;
		flow_entry->fwd_type = entry.bf0.fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			flow_entry->snat_nexthop = entry.bf0.next_hop1;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			flow_entry->dnat_nexthop = entry.bf3.next_hop2;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			flow_entry->route_nexthop = entry.bf2.next_hop3;
			flow_entry->port_valid = entry.bf2.port_vp_valid1;
			flow_entry->route_port = entry.bf2.port_vp1;
			if (entry.bf2.port_vp1 >= 64)
				flow_entry->route_port |= 0x1000000;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			flow_entry->bridge_port = entry.bf1.port_vp2;
			if (entry.bf1.port_vp2 >= 64)
				flow_entry->bridge_port |= 0x1000000;
		}
		flow_entry->deacclr_en = entry.bf0.de_acce;
		flow_entry->copy_tocpu_en = entry.bf0.copy_to_cpu_en;
		flow_entry->syn_toggle = entry.bf0.syn_toggle;
		flow_entry->pri_profile = entry.bf0.pri_profile_0 |\
								entry.bf0.pri_profile_1 << 1;
		flow_entry->sevice_code = entry.bf0.service_code;
		flow_entry->flow_ip.ipv4 = entry.bf0.ip_addr_0 |\
					   entry.bf0.ip_addr_1 << 20;
		flow_entry->ip_type = entry.bf0.ip_protocol;
	} else if ((type & FAL_FLOW_IP6_3TUPLE_ADDR) == FAL_FLOW_IP6_3TUPLE_ADDR) {
		union in_flow_ipv6_3tuple_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_ipv6_3tuple_get(dev_id, get_mode, &flow_entry->entry_id, &entry);
		if (entry.bf0.entry_type != FLOW_ENTRY_TYPE_IPV6 ||
				entry.bf0.protocol_type != FLOW_TUPLE_TYPE_3) {
			return SW_BAD_VALUE;
		}
		flow_entry->host_addr_type = entry.bf0.host_addr_index_type;
		flow_entry->host_addr_index = entry.bf0.host_addr_index;
		flow_entry->protocol = entry.bf0.protocol_type;
		flow_entry->age = entry.bf0.age;
		flow_entry->src_intf_valid = entry.bf0.src_l3_if_valid;
		flow_entry->src_intf_index = entry.bf0.src_l3_if;
		flow_entry->fwd_type = entry.bf0.fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			flow_entry->snat_nexthop = entry.bf1.next_hop1;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			flow_entry->dnat_nexthop = entry.bf3.next_hop2;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			flow_entry->route_nexthop = entry.bf2.next_hop3;
			flow_entry->port_valid = entry.bf2.port_vp_valid1;
			flow_entry->route_port = entry.bf2.port_vp1;
			if (entry.bf2.port_vp1 >= 64)
				flow_entry->route_port |= 0x1000000;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			flow_entry->bridge_port = entry.bf0.port_vp2;
			if (entry.bf0.port_vp2 >= 64)
				flow_entry->bridge_port |= 0x1000000;
		}
		flow_entry->deacclr_en = entry.bf0.de_acce;
		flow_entry->copy_tocpu_en = entry.bf0.copy_to_cpu_en;
		flow_entry->syn_toggle = entry.bf0.syn_toggle;
		flow_entry->pri_profile = entry.bf0.pri_profile_0 |\
								entry.bf0.pri_profile_1 << 1;
		flow_entry->sevice_code = entry.bf0.service_code;
		flow_entry->flow_ip.ipv6.ul[3] = entry.bf0.ip_addr_0 |\
					   entry.bf0.ip_addr_1 << 20;
		flow_entry->flow_ip.ipv6.ul[2] = entry.bf0.ip_addr_1 >> 12 |\
					   entry.bf0.ip_addr_2 << 20;
		flow_entry->flow_ip.ipv6.ul[1] = entry.bf0.ip_addr_2 >> 12 |\
					   entry.bf0.ip_addr_3 << 20;
		flow_entry->flow_ip.ipv6.ul[0] = entry.bf0.ip_addr_3 >> 12 |\
					   entry.bf0.ip_addr_4 << 20;
		flow_entry->ip_type = entry.bf0.ip_protocol;
	} else
		return SW_FAIL;

	if (rv == SW_OK) {
		union eg_flow_tree_map_tbl_u eg_treemap;
		union in_flow_cnt_tbl_u cnt;
		rv = hppe_eg_flow_tree_map_tbl_get(dev_id, flow_entry->entry_id, &eg_treemap);
		flow_entry->tree_id = eg_treemap.bf.tree_id;
		rv = hppe_in_flow_cnt_tbl_get(dev_id, flow_entry->entry_id, &cnt);
		flow_entry->pkt_counter = cnt.bf.hit_pkt_counter;
		flow_entry->byte_counter = cnt.bf.hit_byte_counter_0 | \
					((a_uint64_t)cnt.bf.hit_byte_counter_1 << 32);
	}
	return rv;
}

sw_error_t
adpt_hppe_flow_entry_next(
		a_uint32_t dev_id,
		a_uint32_t next_mode,
		fal_flow_entry_t *flow_entry)
{
	a_uint32_t i = 0, step = 0;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_entry);

	if (FAL_NEXT_ENTRY_FIRST_ID == flow_entry->entry_id)
		i = 0;

	if (next_mode == FAL_FLOW_IP4_3TUPLE_ADDR ||
		next_mode == FAL_FLOW_IP4_5TUPLE_ADDR) {
		if (FAL_NEXT_ENTRY_FIRST_ID != flow_entry->entry_id)
			i = flow_entry->entry_id + 1;
		step = 1;
	} else if (next_mode == FAL_FLOW_IP6_5TUPLE_ADDR ||
		 next_mode == FAL_FLOW_IP6_3TUPLE_ADDR) {
		if (FAL_NEXT_ENTRY_FIRST_ID != flow_entry->entry_id)
			i = (flow_entry->entry_id & ~1) + 2;
		step = 2;
	}
	for (; i < IN_FLOW_TBL_MAX_ENTRY;) {
		flow_entry->entry_type = next_mode;
		flow_entry->entry_id = i;
		rv = adpt_hppe_flow_entry_get(dev_id, 1, flow_entry);
		if (!rv) {
			return rv;
		}
		i += step;
	}

	return SW_FAIL;

}

sw_error_t
adpt_hppe_flow_entry_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;
	a_uint32_t type = 0;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_entry);

	if (del_mode == FAL_FLOW_OP_MODE_FLUSH)
		return hppe_flow_flush_common(dev_id);

	type = flow_entry->entry_type;
	if ((type & FAL_FLOW_IP4_5TUPLE_ADDR) == FAL_FLOW_IP4_5TUPLE_ADDR) {
		union in_flow_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
			entry.bf0.l4_port1 = flow_entry->snat_srcport;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
			entry.bf3.l4_port2 = flow_entry->dnat_dstport;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port & 0xffffff;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port & 0xffffff;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_ipv4_5tuple_del(dev_id, del_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP6_5TUPLE_ADDR) == FAL_FLOW_IP6_5TUPLE_ADDR) {
		union in_flow_ipv6_5tuple_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port & 0xffffff;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port & 0xffffff;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_ipv6_5tuple_del(dev_id, del_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP4_3TUPLE_ADDR) == FAL_FLOW_IP4_3TUPLE_ADDR) {
		union in_flow_3tuple_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port & 0xffffff;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port & 0xffffff;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_ipv4_3tuple_del(dev_id, del_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP6_3TUPLE_ADDR) == FAL_FLOW_IP6_3TUPLE_ADDR) {
		union in_flow_ipv6_3tuple_tbl_u entry;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf1.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port & 0xffffff;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf0.port_vp2 = flow_entry->bridge_port & 0xffffff;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_ipv6_3tuple_del(dev_id, del_mode, &flow_entry->entry_id, &entry);
	} else
		return SW_FAIL;
	return rv;
}
sw_error_t
adpt_hppe_flow_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union flow_ctrl0_u flow_ctrl0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	memset(&flow_ctrl0, 0, sizeof(flow_ctrl0));

	rv = hppe_flow_ctrl0_get(dev_id, &flow_ctrl0);
	if( rv != SW_OK )
		return rv;

	*enable = flow_ctrl0.bf.flow_en;
	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_flow_ctrl_set(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *ctrl)
{
	sw_error_t rv = SW_OK;
	union flow_ctrl1_u flow_ctrl1;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	memset(&flow_ctrl1, 0, sizeof(flow_ctrl1));
	rv = hppe_flow_ctrl1_get(dev_id, type, &flow_ctrl1);
	if( rv != SW_OK )
		return rv;

	if (dir == FAL_FLOW_LAN_TO_LAN_DIR) {
		flow_ctrl1.bf.flow_ctl0_miss_action = ctrl->miss_action;
		flow_ctrl1.bf.flow_ctl0_frag_bypass = ctrl->frag_bypass_en;
		flow_ctrl1.bf.flow_ctl0_tcp_special = ctrl->tcp_spec_bypass_en;
		flow_ctrl1.bf.flow_ctl0_bypass = ctrl->all_bypass_en;
		flow_ctrl1.bf.flow_ctl0_key_sel = ctrl->key_sel;
	} else if (dir == FAL_FLOW_LAN_TO_WAN_DIR) {
		flow_ctrl1.bf.flow_ctl1_miss_action = ctrl->miss_action;
		flow_ctrl1.bf.flow_ctl1_frag_bypass = ctrl->frag_bypass_en;
		flow_ctrl1.bf.flow_ctl1_tcp_special = ctrl->tcp_spec_bypass_en;
		flow_ctrl1.bf.flow_ctl1_bypass = ctrl->all_bypass_en;
		flow_ctrl1.bf.flow_ctl1_key_sel = ctrl->key_sel;
	} else if (dir == FAL_FLOW_WAN_TO_LAN_DIR) {
		flow_ctrl1.bf.flow_ctl2_miss_action = ctrl->miss_action;
		flow_ctrl1.bf.flow_ctl2_frag_bypass = ctrl->frag_bypass_en;
		flow_ctrl1.bf.flow_ctl2_tcp_special = ctrl->tcp_spec_bypass_en;
		flow_ctrl1.bf.flow_ctl2_bypass = ctrl->all_bypass_en;
		flow_ctrl1.bf.flow_ctl2_key_sel = ctrl->key_sel;
	} else if (dir == FAL_FLOW_WAN_TO_WAN_DIR) {
		flow_ctrl1.bf.flow_ctl3_miss_action = ctrl->miss_action;
		flow_ctrl1.bf.flow_ctl3_frag_bypass = ctrl->frag_bypass_en;
		flow_ctrl1.bf.flow_ctl3_tcp_special = ctrl->tcp_spec_bypass_en;
		flow_ctrl1.bf.flow_ctl3_bypass = ctrl->all_bypass_en;
		flow_ctrl1.bf.flow_ctl3_key_sel = ctrl->key_sel;
	} else if (dir == FAL_FLOW_UNKOWN_DIR_DIR) {
		flow_ctrl1.bf.flow_ctl4_miss_action = ctrl->miss_action;
		flow_ctrl1.bf.flow_ctl4_frag_bypass = ctrl->frag_bypass_en;
		flow_ctrl1.bf.flow_ctl4_tcp_special = ctrl->tcp_spec_bypass_en;
		flow_ctrl1.bf.flow_ctl4_bypass = ctrl->all_bypass_en;
		flow_ctrl1.bf.flow_ctl4_key_sel = ctrl->key_sel;
	} else
		return SW_FAIL;

	return hppe_flow_ctrl1_set(dev_id, type, &flow_ctrl1);;
}

#ifndef IN_FLOW_MINI
sw_error_t
adpt_hppe_flow_age_timer_get(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer)
{
	sw_error_t rv = SW_OK;
	union flow_ctrl0_u flow_ctrl0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(age_timer);

	memset(&flow_ctrl0, 0, sizeof(flow_ctrl0));

	rv = hppe_flow_ctrl0_get(dev_id, &flow_ctrl0);
	if( rv != SW_OK )
		return rv;

	age_timer->age_time = flow_ctrl0.bf.flow_age_timer;
	age_timer->unit = flow_ctrl0.bf.flow_age_timer_unit;
	return SW_OK;
}

sw_error_t
adpt_hppe_flow_status_set(a_uint32_t dev_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union flow_ctrl0_u flow_ctrl0;

	ADPT_DEV_ID_CHECK(dev_id);

	memset(&flow_ctrl0, 0, sizeof(flow_ctrl0));

	rv = hppe_flow_ctrl0_get(dev_id, &flow_ctrl0);
	if( rv != SW_OK )
		return rv;

	flow_ctrl0.bf.flow_en = enable;
	return hppe_flow_ctrl0_set(dev_id, &flow_ctrl0);
}
#endif

sw_error_t
adpt_hppe_flow_ctrl_get(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *ctrl)
{
	sw_error_t rv = SW_OK;
	union flow_ctrl1_u flow_ctrl1;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	memset(&flow_ctrl1, 0, sizeof(flow_ctrl1));
	rv = hppe_flow_ctrl1_get(dev_id, type, &flow_ctrl1);
	if( rv != SW_OK )
		return rv;

	if (dir == FAL_FLOW_LAN_TO_LAN_DIR) {
		ctrl->miss_action = flow_ctrl1.bf.flow_ctl0_miss_action;
		ctrl->frag_bypass_en = flow_ctrl1.bf.flow_ctl0_frag_bypass;
		ctrl->tcp_spec_bypass_en = flow_ctrl1.bf.flow_ctl0_tcp_special;
		ctrl->all_bypass_en = flow_ctrl1.bf.flow_ctl0_bypass;
		ctrl->key_sel = flow_ctrl1.bf.flow_ctl0_key_sel;
	} else if (dir == FAL_FLOW_LAN_TO_WAN_DIR) {
		ctrl->miss_action = flow_ctrl1.bf.flow_ctl1_miss_action;
		ctrl->frag_bypass_en = flow_ctrl1.bf.flow_ctl1_frag_bypass;
		ctrl->tcp_spec_bypass_en = flow_ctrl1.bf.flow_ctl1_tcp_special;
		ctrl->all_bypass_en = flow_ctrl1.bf.flow_ctl1_bypass;
		ctrl->key_sel = flow_ctrl1.bf.flow_ctl1_key_sel;
	} else if (dir == FAL_FLOW_WAN_TO_LAN_DIR) {
		ctrl->miss_action = flow_ctrl1.bf.flow_ctl2_miss_action;
		ctrl->frag_bypass_en = flow_ctrl1.bf.flow_ctl2_frag_bypass;
		ctrl->tcp_spec_bypass_en = flow_ctrl1.bf.flow_ctl2_tcp_special;
		ctrl->all_bypass_en = flow_ctrl1.bf.flow_ctl2_bypass;
		ctrl->key_sel = flow_ctrl1.bf.flow_ctl2_key_sel;
	} else if (dir == FAL_FLOW_WAN_TO_WAN_DIR) {
		ctrl->miss_action = flow_ctrl1.bf.flow_ctl3_miss_action;
		ctrl->frag_bypass_en = flow_ctrl1.bf.flow_ctl3_frag_bypass;
		ctrl->tcp_spec_bypass_en = flow_ctrl1.bf.flow_ctl3_tcp_special;
		ctrl->all_bypass_en = flow_ctrl1.bf.flow_ctl3_bypass;
		ctrl->key_sel = flow_ctrl1.bf.flow_ctl3_key_sel;
	} else if (dir == FAL_FLOW_UNKOWN_DIR_DIR) {
		ctrl->miss_action = flow_ctrl1.bf.flow_ctl4_miss_action;
		ctrl->frag_bypass_en = flow_ctrl1.bf.flow_ctl4_frag_bypass;
		ctrl->tcp_spec_bypass_en = flow_ctrl1.bf.flow_ctl4_tcp_special;
		ctrl->all_bypass_en = flow_ctrl1.bf.flow_ctl4_bypass;
		ctrl->key_sel = flow_ctrl1.bf.flow_ctl4_key_sel;
	} else
		return SW_FAIL;

	return SW_OK;
}

#ifndef IN_FLOW_MINI
sw_error_t
adpt_hppe_flow_age_timer_set(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer)
{
	sw_error_t rv = SW_OK;
	union flow_ctrl0_u flow_ctrl0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(age_timer);

	memset(&flow_ctrl0, 0, sizeof(flow_ctrl0));

	rv = hppe_flow_ctrl0_get(dev_id, &flow_ctrl0);
	if( rv != SW_OK )
		return rv;

	flow_ctrl0.bf.flow_age_timer = age_timer->age_time;
	flow_ctrl0.bf.flow_age_timer_unit = age_timer->unit;
	return hppe_flow_ctrl0_set(dev_id, &flow_ctrl0);
}

sw_error_t
adpt_hppe_flow_entry_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode, /*index or hash*/
		fal_flow_entry_t *flow_entry)
{
	sw_error_t rv = SW_OK;
	a_uint32_t type = 0;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(flow_entry);

	type = flow_entry->entry_type;
	if ((type & FAL_FLOW_IP4_5TUPLE_ADDR) == FAL_FLOW_IP4_5TUPLE_ADDR) {
		union in_flow_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
			entry.bf0.l4_port1 = flow_entry->snat_srcport;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
			entry.bf3.l4_port2 = flow_entry->dnat_dstport;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port & 0xffffff;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port & 0xffffff;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_ipv4_5tuple_add(dev_id, (a_uint32_t)add_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP6_5TUPLE_ADDR) == FAL_FLOW_IP6_5TUPLE_ADDR) {
		union in_flow_ipv6_5tuple_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port & 0xffffff;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port & 0xffffff;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.l4_sport = flow_entry->src_port;
		entry.bf0.l4_dport_0 = flow_entry->dst_port;
		entry.bf0.l4_dport_1 = flow_entry->dst_port >> 4;
		rv = hppe_flow_ipv6_5tuple_add(dev_id, (a_uint32_t)add_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP4_3TUPLE_ADDR) == FAL_FLOW_IP4_3TUPLE_ADDR) {
		union in_flow_3tuple_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV4;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf0.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port & 0xffffff;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf1.port_vp2 = flow_entry->bridge_port & 0xffffff;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv4;
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv4 >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_ipv4_3tuple_add(dev_id, (a_uint32_t)add_mode, &flow_entry->entry_id, &entry);
	} else if ((type & FAL_FLOW_IP6_3TUPLE_ADDR) == FAL_FLOW_IP6_3TUPLE_ADDR) {
		union in_flow_ipv6_3tuple_tbl_u entry;
		entry.bf0.valid= 1;
		entry.bf0.entry_type = FLOW_ENTRY_TYPE_IPV6;
		entry.bf0.host_addr_index_type = flow_entry->host_addr_type;
		entry.bf0.host_addr_index = flow_entry->host_addr_index;
		entry.bf0.protocol_type = flow_entry->protocol;
		entry.bf0.age = flow_entry->age;
		entry.bf0.src_l3_if_valid = flow_entry->src_intf_valid;
		entry.bf0.src_l3_if = flow_entry->src_intf_index;
		entry.bf0.fwd_type = flow_entry->fwd_type;
		if (flow_entry->fwd_type == FAL_FLOW_SNAT) {
			entry.bf1.next_hop1 = flow_entry->snat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_DNAT) {
			entry.bf3.next_hop2 = flow_entry->dnat_nexthop;
		} else if (flow_entry->fwd_type == FAL_FLOW_ROUTE) {
			entry.bf2.next_hop3 = flow_entry->route_nexthop;
			entry.bf2.port_vp_valid1= flow_entry->port_valid;
			entry.bf2.port_vp1 = flow_entry->route_port & 0xffffff;
		} else if (flow_entry->fwd_type == FAL_FLOW_BRIDGE) {
			entry.bf0.port_vp2 = flow_entry->bridge_port & 0xffffff;
		}
		entry.bf0.de_acce = flow_entry->deacclr_en;
		entry.bf0.copy_to_cpu_en = flow_entry->copy_tocpu_en;
		entry.bf0.syn_toggle = flow_entry->syn_toggle;
		entry.bf0.pri_profile_0 = flow_entry->pri_profile;
		entry.bf0.pri_profile_1 = flow_entry->pri_profile >> 1;
		entry.bf0.service_code = flow_entry->sevice_code;
		entry.bf0.ip_addr_0 = flow_entry->flow_ip.ipv6.ul[3];
		entry.bf0.ip_addr_1 = flow_entry->flow_ip.ipv6.ul[3] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[2] << 12;
		entry.bf0.ip_addr_2 = flow_entry->flow_ip.ipv6.ul[2] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[1] << 12;
		entry.bf0.ip_addr_3 = flow_entry->flow_ip.ipv6.ul[1] >> 20 |\
							flow_entry->flow_ip.ipv6.ul[0] << 12;
		entry.bf0.ip_addr_4 = flow_entry->flow_ip.ipv6.ul[0] >> 20;
		entry.bf0.ip_protocol = flow_entry->ip_type;
		rv = hppe_flow_ipv6_3tuple_add(dev_id, (a_uint32_t)add_mode, &flow_entry->entry_id, &entry);
	} else
		return SW_FAIL;
	if (rv == SW_OK) {
		union eg_flow_tree_map_tbl_u eg_treemap;
		eg_treemap.bf.tree_id = flow_entry->tree_id;
		rv = hppe_eg_flow_tree_map_tbl_set(dev_id, flow_entry->entry_id, &eg_treemap);
	}
	return rv;
}

sw_error_t
adpt_hppe_flow_global_cfg_get(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;
#if defined(CPPE)
	a_uint32_t chip_ver = 0;
	a_bool_t flow_cpy_escape = A_FALSE;
#endif
	union flow_ctrl0_u flow_ctrl0;
	union l3_route_ctrl_u route_ctrl;
	union l3_route_ctrl_ext_u route_ctrl_ext;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cfg);
	
	memset(&flow_ctrl0, 0, sizeof(flow_ctrl0));
	memset(&route_ctrl, 0, sizeof(route_ctrl));
	memset(&route_ctrl_ext, 0, sizeof(route_ctrl_ext));

	rv = hppe_flow_ctrl0_get(dev_id, &flow_ctrl0);
	if( rv != SW_OK )
		return rv;
	rv = hppe_l3_route_ctrl_get(dev_id, &route_ctrl);
	if( rv != SW_OK )
		return rv;
	rv = hppe_l3_route_ctrl_ext_get(dev_id, &route_ctrl_ext);
	if( rv != SW_OK )
		return rv;

#if defined(CPPE)
	chip_ver = adpt_hppe_chip_revision_get(dev_id);
	if (chip_ver == CPPE_REVISION) {
		rv = adpt_cppe_flow_copy_escape_get(dev_id, &flow_cpy_escape);
		SW_RTN_ON_ERROR(rv);
		cfg->flow_mismatch_copy_escape_en = flow_cpy_escape;
	}
#endif

	cfg->src_if_check_action= route_ctrl.bf.flow_src_if_check_cmd;
	cfg->src_if_check_deacclr_en= route_ctrl.bf.flow_src_if_check_de_acce;
	cfg->service_loop_en = route_ctrl_ext.bf.flow_service_code_loop_en;
	cfg->service_loop_action = route_ctrl.bf.flow_service_code_loop;
	cfg->service_loop_deacclr_en = route_ctrl.bf.flow_service_code_loop_de_acce;
	cfg->flow_deacclr_action = route_ctrl.bf.flow_de_acce_cmd;
	cfg->sync_mismatch_action = route_ctrl.bf.flow_sync_mismatch_cmd;
	cfg->sync_mismatch_deacclr_en = route_ctrl.bf.flow_sync_mismatch_de_acce;
	cfg->hash_mode_0 = flow_ctrl0.bf.flow_hash_mode_0;
	cfg->hash_mode_1 = flow_ctrl0.bf.flow_hash_mode_1;

	return SW_OK;
}

sw_error_t
adpt_hppe_flow_global_cfg_set(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;
#if defined(CPPE)
	a_uint32_t chip_ver = 0;
#endif
	union flow_ctrl0_u flow_ctrl0;
	union l3_route_ctrl_u route_ctrl;
	union l3_route_ctrl_ext_u route_ctrl_ext;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cfg);
	
	memset(&flow_ctrl0, 0, sizeof(flow_ctrl0));
	memset(&route_ctrl, 0, sizeof(route_ctrl));
	memset(&route_ctrl_ext, 0, sizeof(route_ctrl_ext));

	rv = hppe_flow_ctrl0_get(dev_id, &flow_ctrl0);
	SW_RTN_ON_ERROR(rv);

	rv = hppe_l3_route_ctrl_get(dev_id, &route_ctrl);
	SW_RTN_ON_ERROR(rv);

	rv = hppe_l3_route_ctrl_ext_get(dev_id, &route_ctrl_ext);
	SW_RTN_ON_ERROR(rv);

	route_ctrl.bf.flow_src_if_check_cmd = cfg->src_if_check_action;
	route_ctrl.bf.flow_src_if_check_de_acce = cfg->src_if_check_deacclr_en;
	route_ctrl_ext.bf.flow_service_code_loop_en = cfg->service_loop_en;
	route_ctrl.bf.flow_service_code_loop = cfg->service_loop_action;
	route_ctrl.bf.flow_service_code_loop_de_acce = cfg->service_loop_deacclr_en;
	route_ctrl.bf.flow_de_acce_cmd = cfg->flow_deacclr_action;
	route_ctrl.bf.flow_sync_mismatch_cmd = cfg->sync_mismatch_action;
	route_ctrl.bf.flow_sync_mismatch_de_acce = cfg->sync_mismatch_deacclr_en;
	flow_ctrl0.bf.flow_hash_mode_0 = cfg->hash_mode_0;
	flow_ctrl0.bf.flow_hash_mode_1 = cfg->hash_mode_1;

	rv = hppe_flow_ctrl0_set(dev_id, &flow_ctrl0);
	SW_RTN_ON_ERROR(rv);

	rv = hppe_l3_route_ctrl_set(dev_id, &route_ctrl);
	SW_RTN_ON_ERROR(rv);

	rv = hppe_l3_route_ctrl_ext_set(dev_id, &route_ctrl_ext);
	SW_RTN_ON_ERROR(rv);

#if defined(CPPE)
	chip_ver = adpt_hppe_chip_revision_get(dev_id);
	if (chip_ver == CPPE_REVISION) {
		rv = adpt_cppe_flow_copy_escape_set(dev_id,
				cfg->flow_mismatch_copy_escape_en);
		SW_RTN_ON_ERROR(rv);
	}
#endif

	return SW_OK;
}
#endif

void adpt_hppe_flow_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_flow_func_bitmap = 0;
	return;
}

static void adpt_hppe_flow_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_flow_host_add = NULL;
	p_adpt_api->adpt_flow_entry_get = NULL;
	p_adpt_api->adpt_flow_entry_del = NULL;
	p_adpt_api->adpt_flow_status_get = NULL;
	p_adpt_api->adpt_flow_ctrl_set = NULL;
	p_adpt_api->adpt_flow_age_timer_get = NULL;
	p_adpt_api->adpt_flow_status_set = NULL;
	p_adpt_api->adpt_flow_host_get = NULL;
	p_adpt_api->adpt_flow_host_del = NULL;
	p_adpt_api->adpt_flow_ctrl_get = NULL;
	p_adpt_api->adpt_flow_age_timer_set = NULL;
	p_adpt_api->adpt_flow_entry_add = NULL;
	p_adpt_api->adpt_flow_global_cfg_get = NULL;
	p_adpt_api->adpt_flow_global_cfg_set = NULL;
	p_adpt_api->adpt_flow_entry_next = NULL;

	return;
}

sw_error_t adpt_hppe_flow_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_flow_func_unregister(dev_id, p_adpt_api);

#ifndef IN_FLOW_MINI
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_HOST_ADD))
		p_adpt_api->adpt_flow_host_add = adpt_hppe_flow_host_add;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_ENTRY_GET))
		p_adpt_api->adpt_flow_entry_get = adpt_hppe_flow_entry_get;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_ENTRY_DEL))
		p_adpt_api->adpt_flow_entry_del = adpt_hppe_flow_entry_del;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_STATUS_GET))
		p_adpt_api->adpt_flow_status_get = adpt_hppe_flow_status_get;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_AGE_TIMER_GET))
		p_adpt_api->adpt_flow_age_timer_get = adpt_hppe_flow_age_timer_get;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_STATUS_SET))
		p_adpt_api->adpt_flow_status_set = adpt_hppe_flow_status_set;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_HOST_GET))
		p_adpt_api->adpt_flow_host_get = adpt_hppe_flow_host_get;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_HOST_DEL))
		p_adpt_api->adpt_flow_host_del = adpt_hppe_flow_host_del;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_AGE_TIMER_SET))
		p_adpt_api->adpt_flow_age_timer_set = adpt_hppe_flow_age_timer_set;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_ENTRY_ADD))
		p_adpt_api->adpt_flow_entry_add = adpt_hppe_flow_entry_add;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_GLOBAL_CFG_GET))
		p_adpt_api->adpt_flow_global_cfg_get = adpt_hppe_flow_global_cfg_get;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_GLOBAL_CFG_SET))
		p_adpt_api->adpt_flow_global_cfg_set = adpt_hppe_flow_global_cfg_set;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_ENTRY_NEXT))
		p_adpt_api->adpt_flow_entry_next = adpt_hppe_flow_entry_next;
#endif
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_CTRL_GET))
		p_adpt_api->adpt_flow_ctrl_get = adpt_hppe_flow_ctrl_get;
	if (p_adpt_api->adpt_flow_func_bitmap & (1 << FUNC_FLOW_CTRL_SET))
		p_adpt_api->adpt_flow_ctrl_set = adpt_hppe_flow_ctrl_set;

	return SW_OK;
}

/**
 * @}
 */
