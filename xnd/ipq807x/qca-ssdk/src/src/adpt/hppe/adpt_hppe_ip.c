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


/**
 * @defgroup
 * @{
 */
#include "sw.h"
#include "fal_ip.h"
#include "hppe_ip_reg.h"
#include "hppe_ip.h"
#include "adpt.h"

#ifndef IN_IP_MINI
sw_error_t
adpt_hppe_ip_network_route_get(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type,
			fal_network_route_entry_t *entry)
{
	sw_error_t rv = SW_OK;
	union network_route_ip_u network_route_ip;
	union network_route_ip_ext_u network_route_ip_ext;
	union network_route_action_u network_route_action;
	fal_ip6_addr_t ipv6, ipv6_mask;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);
	memset(&network_route_ip, 0, sizeof(network_route_ip));
	memset(&network_route_ip_ext, 0, sizeof(network_route_ip_ext));
	memset(&network_route_action, 0, sizeof(network_route_action));

	if (type > 1 || (type == 0 && index > 31) || (type == 1 && index > 7))
		return SW_BAD_VALUE;

	if (type == 0) {
		rv = hppe_network_route_ip_get(dev_id, index,
							&network_route_ip);
		if( rv != SW_OK )
			return rv;
		rv = hppe_network_route_ip_ext_get(dev_id, index,
							&network_route_ip_ext);
		if( rv != SW_OK )
			return rv;
		rv = hppe_network_route_action_get(dev_id, index,
							&network_route_action);
		if( rv != SW_OK )
			return rv;
	} else {
		rv = hppe_network_route_ip_get(dev_id, index * 4, &network_route_ip);
		if( rv != SW_OK )
			return rv;
		ipv6.ul[3] = network_route_ip.val[0];
		ipv6.ul[2] = network_route_ip.val[1];
		rv = hppe_network_route_ip_get(dev_id, index * 4 + 1, &network_route_ip);
		if( rv != SW_OK )
			return rv;
		ipv6.ul[1] = network_route_ip.val[0];
		ipv6.ul[0] = network_route_ip.val[1];
		rv = hppe_network_route_ip_get(dev_id, index * 4 + 2, &network_route_ip);
		if( rv != SW_OK )
			return rv;
		ipv6_mask.ul[3] = network_route_ip.val[0];
		ipv6_mask.ul[2] = network_route_ip.val[1];
		rv = hppe_network_route_ip_get(dev_id, index * 4 + 3, &network_route_ip);
		if( rv != SW_OK )
			return rv;
		ipv6_mask.ul[1] = network_route_ip.val[0];
		ipv6_mask.ul[0] = network_route_ip.val[1];
		rv = hppe_network_route_ip_ext_get(dev_id, index * 4,
							&network_route_ip_ext);
		if( rv != SW_OK )
			return rv;
		rv = hppe_network_route_action_get(dev_id, index * 4,
							&network_route_action);
		if( rv != SW_OK )
			return rv;
	}

	if (!network_route_ip_ext.bf.valid)
		return SW_FAIL;
	entry->action = (fal_fwd_cmd_t)network_route_action.bf.fwd_cmd;
	entry->lan_wan = network_route_action.bf.lan_wan;
	entry->dst_info = network_route_action.bf.dst_info;
	entry->type = network_route_ip_ext.bf.entry_type;
	if (type == 0) {
		entry->route_addr.ip4_addr = network_route_ip.bf.ip_addr;
		entry->route_addr_mask.ip4_addr_mask = network_route_ip.bf.ip_addr_mask;
	} else {
		memcpy(&entry->route_addr.ip6_addr , &ipv6, sizeof(ipv6));
		memcpy(&entry->route_addr_mask.ip6_addr_mask,
							&ipv6_mask, sizeof(ipv6_mask));
	}

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * host_entry)
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
		entry.bf.dst_info = host_entry->dst_info;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ip_addr = host_entry->ip4_addr;
		rv = hppe_host_ipv4_add(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	} else if ((type & FAL_IP_IP6_ADDR) == FAL_IP_IP6_ADDR) {
		union host_ipv6_tbl_u entry;
		entry.bf.valid= host_entry->status;
		entry.bf.key_type = 2;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->dst_info;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ipv6_addr_0 = host_entry->ip6_addr.ul[3];
		entry.bf.ipv6_addr_1 = host_entry->ip6_addr.ul[3] >> 10 | \
							host_entry->ip6_addr.ul[2] << 22;
		entry.bf.ipv6_addr_2 = host_entry->ip6_addr.ul[2] >> 10 | \
							host_entry->ip6_addr.ul[1] << 22;
		entry.bf.ipv6_addr_3 = host_entry->ip6_addr.ul[1] >> 10 | \
							host_entry->ip6_addr.ul[0] << 22;
		entry.bf.ipv6_addr_4 = host_entry->ip6_addr.ul[0] >> 10;
		rv = hppe_host_ipv6_add(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	} else if ((type & FAL_IP_IP4_ADDR_MCAST) == FAL_IP_IP4_ADDR_MCAST) {
		union host_ipv4_mcast_tbl_u entry;
		entry.bf.valid= host_entry->status;
		entry.bf.key_type = 1;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->dst_info;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.gip_addr_0 = host_entry->ip4_addr;
		entry.bf.gip_addr_1 = host_entry->ip4_addr >> 11;
		entry.bf.vsi = host_entry->mcast_info.vsi;
		entry.bf.sip_addr_0 = host_entry->mcast_info.sip4_addr;
		entry.bf.sip_addr_1 = host_entry->mcast_info.sip4_addr >> 11;
		rv = hppe_host_ipv4_mcast_add(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	} else if ((type & FAL_IP_IP6_ADDR_MCAST) == FAL_IP_IP6_ADDR_MCAST) {
		union host_ipv6_mcast_tbl_u entry;
		entry.bf.valid= host_entry->status;
		entry.bf.key_type = 3;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->dst_info;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.gipv6_addr_0 = host_entry->ip6_addr.ul[3];
		entry.bf.gipv6_addr_1 = host_entry->ip6_addr.ul[3] >> 20 | \
							host_entry->ip6_addr.ul[2] << 12;
		entry.bf.gipv6_addr_2 = host_entry->ip6_addr.ul[2] >> 20 | \
							host_entry->ip6_addr.ul[1] << 12;
		entry.bf.gipv6_addr_3 = host_entry->ip6_addr.ul[1] >> 20 | \
							host_entry->ip6_addr.ul[0] << 12;
		entry.bf.gipv6_addr_4 = host_entry->ip6_addr.ul[0] >> 20;
		entry.bf.vsi = host_entry->mcast_info.vsi;
		entry.bf.sipv6_addr_0 = host_entry->mcast_info.sip6_addr.ul[3];
		entry.bf.sipv6_addr_1 = host_entry->mcast_info.sip6_addr.ul[3] >> 20 | \
							host_entry->mcast_info.sip6_addr.ul[2] << 12;
		entry.bf.sipv6_addr_2 = host_entry->mcast_info.sip6_addr.ul[2] >> 20 | \
							host_entry->mcast_info.sip6_addr.ul[1] << 12;
		entry.bf.sipv6_addr_3 = host_entry->mcast_info.sip6_addr.ul[1] >> 20 | \
							host_entry->mcast_info.sip6_addr.ul[0] << 12;
		entry.bf.sipv6_addr_4 = host_entry->mcast_info.sip6_addr.ul[0] >> 20;
		rv = hppe_host_ipv6_mcast_add(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	}
	return rv;
}
sw_error_t
adpt_hppe_ip_vsi_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg)
{
	sw_error_t rv = SW_OK;
	union l3_vsi_ext_u l3_vsi_ext;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(sg_cfg);
	memset(&l3_vsi_ext, 0, sizeof(l3_vsi_ext));

	rv = hppe_l3_vsi_ext_get(dev_id, vsi, &l3_vsi_ext);
	if( rv != SW_OK )
		return rv;

	sg_cfg->ipv4_sg_en = l3_vsi_ext.bf.ipv4_sg_en;
	sg_cfg->ipv4_sg_vio_action = l3_vsi_ext.bf.ipv4_sg_vio_cmd;
	sg_cfg->ipv4_sg_port_en = l3_vsi_ext.bf.ipv4_sg_port_en;
	sg_cfg->ipv4_sg_svlan_en = l3_vsi_ext.bf.ipv4_sg_svlan_en;
	sg_cfg->ipv4_sg_cvlan_en = l3_vsi_ext.bf.ipv4_sg_cvlan_en;
	sg_cfg->ipv4_src_unk_action = l3_vsi_ext.bf.ipv4_src_unk_cmd;
	sg_cfg->ipv6_sg_en = l3_vsi_ext.bf.ipv6_sg_en;
	sg_cfg->ipv6_sg_vio_action = l3_vsi_ext.bf.ipv6_sg_vio_cmd;
	sg_cfg->ipv6_sg_port_en = l3_vsi_ext.bf.ipv6_sg_port_en;
	sg_cfg->ipv6_sg_svlan_en = l3_vsi_ext.bf.ipv6_sg_svlan_en;
	sg_cfg->ipv6_sg_cvlan_en = l3_vsi_ext.bf.ipv6_sg_cvlan_en;
	sg_cfg->ipv6_src_unk_action = l3_vsi_ext.bf.ipv6_src_unk_cmd;

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_port_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg)
{
	union l3_vp_port_tbl_u l3_vp_port_tbl;

	memset(&l3_vp_port_tbl, 0, sizeof(l3_vp_port_tbl));
	ADPT_DEV_ID_CHECK(dev_id);

	hppe_l3_vp_port_tbl_get(dev_id, port_id, &l3_vp_port_tbl);

	l3_vp_port_tbl.bf.ipv4_sg_en = sg_cfg->ipv4_sg_en;
	l3_vp_port_tbl.bf.ipv4_sg_vio_cmd = sg_cfg->ipv4_sg_vio_action;
	l3_vp_port_tbl.bf.ipv4_sg_port_en = sg_cfg->ipv4_sg_port_en;
	l3_vp_port_tbl.bf.ipv4_sg_svlan_en = sg_cfg->ipv4_sg_svlan_en;
	l3_vp_port_tbl.bf.ipv4_sg_cvlan_en = sg_cfg->ipv4_sg_cvlan_en;
	l3_vp_port_tbl.bf.ipv4_src_unk_cmd = sg_cfg->ipv4_src_unk_action;
	l3_vp_port_tbl.bf.ipv6_sg_en = sg_cfg->ipv6_sg_en;
	l3_vp_port_tbl.bf.ipv6_sg_vio_cmd = sg_cfg->ipv6_sg_vio_action;
	l3_vp_port_tbl.bf.ipv6_sg_port_en = sg_cfg->ipv6_sg_port_en;
	l3_vp_port_tbl.bf.ipv6_sg_svlan_en = sg_cfg->ipv6_sg_svlan_en;
	l3_vp_port_tbl.bf.ipv6_sg_cvlan_en = sg_cfg->ipv6_sg_cvlan_en;
	l3_vp_port_tbl.bf.ipv6_src_unk_cmd = sg_cfg->ipv6_src_unk_action;
	
	return hppe_l3_vp_port_tbl_set(dev_id, port_id, &l3_vp_port_tbl);
}
sw_error_t
adpt_hppe_ip_port_intf_get(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id)
{
	sw_error_t rv = SW_OK;
	union l3_vp_port_tbl_u l3_vp_port_tbl;

	memset(&l3_vp_port_tbl, 0, sizeof(l3_vp_port_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(id);

	rv = hppe_l3_vp_port_tbl_get(dev_id, port_id, &l3_vp_port_tbl);
	if( rv != SW_OK )
		return rv;

	id->l3_if_valid = l3_vp_port_tbl.bf.l3_if_valid;
	id->l3_if_index = l3_vp_port_tbl.bf.l3_if_index;

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_vsi_arp_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
	union l3_vsi_ext_u l3_vsi_ext;

	memset(&l3_vsi_ext, 0, sizeof(l3_vsi_ext));
	ADPT_DEV_ID_CHECK(dev_id);

	hppe_l3_vsi_ext_get(dev_id, vsi, &l3_vsi_ext);

	l3_vsi_ext.bf.ip_arp_sg_en = arp_sg_cfg->ipv4_arp_sg_en;
	l3_vsi_ext.bf.ip_arp_sg_vio_cmd = arp_sg_cfg->ipv4_arp_sg_vio_action;
	l3_vsi_ext.bf.ip_arp_sg_port_en = arp_sg_cfg->ipv4_arp_sg_port_en;
	l3_vsi_ext.bf.ip_arp_sg_svlan_en = arp_sg_cfg->ipv4_arp_sg_svlan_en;
	l3_vsi_ext.bf.ip_arp_sg_cvlan_en = arp_sg_cfg->ipv4_arp_sg_cvlan_en;
	l3_vsi_ext.bf.ip_arp_src_unk_cmd = arp_sg_cfg->ipv4_arp_src_unk_action;
	l3_vsi_ext.bf.ip_nd_sg_en = arp_sg_cfg->ip_nd_sg_en;
	l3_vsi_ext.bf.ip_nd_sg_vio_cmd = arp_sg_cfg->ip_nd_sg_vio_action;
	l3_vsi_ext.bf.ip_nd_sg_port_en = arp_sg_cfg->ip_nd_sg_port_en;
	l3_vsi_ext.bf.ip_nd_sg_svlan_en = arp_sg_cfg->ip_nd_sg_svlan_en;
	l3_vsi_ext.bf.ip_nd_sg_cvlan_en = arp_sg_cfg->ip_nd_sg_cvlan_en;
	l3_vsi_ext.bf.ip_nd_src_unk_cmd = arp_sg_cfg->ip_nd_src_unk_action;
	
	return hppe_l3_vsi_ext_set(dev_id, vsi, &l3_vsi_ext);
}
sw_error_t
adpt_hppe_ip_pub_addr_get(a_uint32_t dev_id,
		a_uint32_t index, fal_ip_pub_addr_t *entry)
{
	sw_error_t rv = SW_OK;
	union in_pub_ip_addr_tbl_u in_pub_ip_addr_tbl;

	memset(&in_pub_ip_addr_tbl, 0, sizeof(in_pub_ip_addr_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	rv = hppe_in_pub_ip_addr_tbl_get(dev_id, index, &in_pub_ip_addr_tbl);
	if( rv != SW_OK )
		return rv;

	entry->pub_ip_addr = in_pub_ip_addr_tbl.bf.ip_addr;
	return SW_OK;
}

sw_error_t
adpt_hppe_ip_port_intf_set(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id)
{
	union l3_vp_port_tbl_u l3_vp_port_tbl;

	memset(&l3_vp_port_tbl, 0, sizeof(l3_vp_port_tbl));
	ADPT_DEV_ID_CHECK(dev_id);

	hppe_l3_vp_port_tbl_get(dev_id, port_id, &l3_vp_port_tbl);

	l3_vp_port_tbl.bf.l3_if_valid = id->l3_if_valid;
	l3_vp_port_tbl.bf.l3_if_index = id->l3_if_index;
	
	return hppe_l3_vp_port_tbl_set(dev_id, port_id, &l3_vp_port_tbl);
}

sw_error_t
adpt_hppe_ip_vsi_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg)
{
	union l3_vsi_ext_u l3_vsi_ext;

	memset(&l3_vsi_ext, 0, sizeof(l3_vsi_ext));
	ADPT_DEV_ID_CHECK(dev_id);

	hppe_l3_vsi_ext_get(dev_id, vsi, &l3_vsi_ext);

	l3_vsi_ext.bf.ipv4_sg_en = sg_cfg->ipv4_sg_en;
	l3_vsi_ext.bf.ipv4_sg_vio_cmd = sg_cfg->ipv4_sg_vio_action;
	l3_vsi_ext.bf.ipv4_sg_port_en = sg_cfg->ipv4_sg_port_en;
	l3_vsi_ext.bf.ipv4_sg_svlan_en = sg_cfg->ipv4_sg_svlan_en;
	l3_vsi_ext.bf.ipv4_sg_cvlan_en = sg_cfg->ipv4_sg_cvlan_en;
	l3_vsi_ext.bf.ipv4_src_unk_cmd = sg_cfg->ipv4_src_unk_action;
	l3_vsi_ext.bf.ipv6_sg_en = sg_cfg->ipv6_sg_en;
	l3_vsi_ext.bf.ipv6_sg_vio_cmd = sg_cfg->ipv6_sg_vio_action;
	l3_vsi_ext.bf.ipv6_sg_port_en = sg_cfg->ipv6_sg_port_en;
	l3_vsi_ext.bf.ipv6_sg_svlan_en = sg_cfg->ipv6_sg_svlan_en;
	l3_vsi_ext.bf.ipv6_sg_cvlan_en = sg_cfg->ipv6_sg_cvlan_en;
	l3_vsi_ext.bf.ipv6_src_unk_cmd = sg_cfg->ipv6_src_unk_action;
	
	return hppe_l3_vsi_ext_set(dev_id, vsi, &l3_vsi_ext);
}
sw_error_t
adpt_hppe_ip_port_macaddr_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr)
{
	union l3_vp_port_tbl_u l3_vp_port_tbl;

	memset(&l3_vp_port_tbl, 0, sizeof(l3_vp_port_tbl));
	ADPT_DEV_ID_CHECK(dev_id);

	hppe_l3_vp_port_tbl_get(dev_id, port_id, &l3_vp_port_tbl);

	l3_vp_port_tbl.bf.mac_valid = macaddr->valid;
	l3_vp_port_tbl.bf.mac_da_0 =  macaddr->mac_addr.uc[4] << 8 | \
							macaddr->mac_addr.uc[5];
	l3_vp_port_tbl.bf.mac_da_1 =  macaddr->mac_addr.uc[0] << 24 | \
							macaddr->mac_addr.uc[1] << 16 | \
							macaddr->mac_addr.uc[2] << 8 | \
							macaddr->mac_addr.uc[3];
	
	return hppe_l3_vp_port_tbl_set(dev_id, port_id, &l3_vp_port_tbl);
}
sw_error_t
adpt_hppe_ip_vsi_intf_get(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id)
{
	sw_error_t rv = SW_OK;
	union l3_vsi_u l3_vsi;

	memset(&l3_vsi, 0, sizeof(l3_vsi));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(id);


	rv = hppe_l3_vsi_get(dev_id, vsi, &l3_vsi);

	if( rv != SW_OK )
		return rv;

	id->l3_if_valid = l3_vsi.bf.l3_if_valid;
	id->l3_if_index = l3_vsi.bf.l3_if_index;

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_network_route_add(a_uint32_t dev_id,
			a_uint32_t index,
			fal_network_route_entry_t *entry)
{
	union network_route_ip_u network_route_ip;
	union network_route_ip_ext_u network_route_ip_ext;
	union network_route_action_u network_route_action;

	memset(&network_route_ip, 0, sizeof(network_route_ip));
	memset(&network_route_ip_ext, 0, sizeof(network_route_ip_ext));
	memset(&network_route_action, 0, sizeof(network_route_action));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	if (entry->type > 1 || (entry->type == 0 && index > 31) || (entry->type == 1 && index > 7))
		return SW_BAD_VALUE;

	if (entry->type == 0) {
		network_route_ip.bf.ip_addr = entry->route_addr.ip4_addr;
		network_route_ip.bf.ip_addr_mask = entry->route_addr_mask.ip4_addr_mask;
		network_route_ip_ext.bf.valid = 1;
		network_route_ip_ext.bf.entry_type = entry->type;
		network_route_action.bf.dst_info = entry->dst_info;
		network_route_action.bf.fwd_cmd = entry->action;
		network_route_action.bf.lan_wan = entry->lan_wan;
		hppe_network_route_ip_set(dev_id, index, &network_route_ip);
		hppe_network_route_ip_ext_set(dev_id, index, &network_route_ip_ext);
		hppe_network_route_action_set(dev_id, index, &network_route_action);
	} else {
		network_route_ip_ext.bf.valid = 1;
		network_route_ip_ext.bf.entry_type = entry->type;
		network_route_action.bf.dst_info = entry->dst_info;
		network_route_action.bf.fwd_cmd = entry->action;
		network_route_action.bf.lan_wan = entry->lan_wan;
		hppe_network_route_ip_ext_set(dev_id, index * 4, &network_route_ip_ext);
		hppe_network_route_action_set(dev_id, index * 4, &network_route_action);
		network_route_ip.bf.ip_addr = entry->route_addr.ip6_addr.ul[3];
		network_route_ip.bf.ip_addr_mask = entry->route_addr.ip6_addr.ul[2];
		hppe_network_route_ip_set(dev_id, index * 4, &network_route_ip);
		network_route_ip.bf.ip_addr = entry->route_addr.ip6_addr.ul[1];
		network_route_ip.bf.ip_addr_mask = entry->route_addr.ip6_addr.ul[0];
		hppe_network_route_ip_set(dev_id, index * 4 + 1, &network_route_ip);
		network_route_ip.bf.ip_addr = entry->route_addr_mask.ip6_addr_mask.ul[3];
		network_route_ip.bf.ip_addr_mask = entry->route_addr_mask.ip6_addr_mask.ul[2];
		hppe_network_route_ip_set(dev_id, index * 4 + 2, &network_route_ip);
		network_route_ip.bf.ip_addr = entry->route_addr_mask.ip6_addr_mask.ul[1];
		network_route_ip.bf.ip_addr_mask = entry->route_addr_mask.ip6_addr_mask.ul[0];
		hppe_network_route_ip_set(dev_id, index * 4 + 3, &network_route_ip);
	}
	return SW_OK;
}

sw_error_t
adpt_hppe_ip_network_route_del(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type)
{
	union network_route_ip_u network_route_ip;
	union network_route_ip_ext_u network_route_ip_ext;
	union network_route_action_u network_route_action;

	memset(&network_route_ip, 0, sizeof(network_route_ip));
	memset(&network_route_ip_ext, 0, sizeof(network_route_ip_ext));
	memset(&network_route_action, 0, sizeof(network_route_action));
	ADPT_DEV_ID_CHECK(dev_id);

	if (type > 1 || (type == 0 && index > 31) || (type == 1 && index > 7))
		return SW_BAD_VALUE;

	network_route_ip_ext.bf.valid = 0;
	if (type == 0) {
		hppe_network_route_ip_ext_set(dev_id, index, &network_route_ip_ext);
	} else {
		hppe_network_route_ip_ext_set(dev_id, index * 4, &network_route_ip_ext);
	}
	return SW_OK;
}

sw_error_t
adpt_hppe_ip_port_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg)
{
	sw_error_t rv = SW_OK;
	union l3_vp_port_tbl_u l3_vp_port_tbl;

	memset(&l3_vp_port_tbl, 0, sizeof(l3_vp_port_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(sg_cfg);

	rv = hppe_l3_vp_port_tbl_get(dev_id, port_id, &l3_vp_port_tbl);
	if( rv != SW_OK )
		return rv;

	sg_cfg->ipv4_sg_en = l3_vp_port_tbl.bf.ipv4_sg_en;
	sg_cfg->ipv4_sg_vio_action= l3_vp_port_tbl.bf.ipv4_sg_vio_cmd;
	sg_cfg->ipv4_sg_port_en = l3_vp_port_tbl.bf.ipv4_sg_port_en;
	sg_cfg->ipv4_sg_svlan_en = l3_vp_port_tbl.bf.ipv4_sg_svlan_en;
	sg_cfg->ipv4_sg_cvlan_en = l3_vp_port_tbl.bf.ipv4_sg_cvlan_en;
	sg_cfg->ipv4_src_unk_action = l3_vp_port_tbl.bf.ipv4_src_unk_cmd;
	sg_cfg->ipv6_sg_en = l3_vp_port_tbl.bf.ipv6_sg_en;
	sg_cfg->ipv6_sg_vio_action = l3_vp_port_tbl.bf.ipv6_sg_vio_cmd;
	sg_cfg->ipv6_sg_port_en = l3_vp_port_tbl.bf.ipv6_sg_port_en;
	sg_cfg->ipv6_sg_svlan_en = l3_vp_port_tbl.bf.ipv6_sg_svlan_en;
	sg_cfg->ipv6_sg_cvlan_en = l3_vp_port_tbl.bf.ipv6_sg_cvlan_en;
	sg_cfg->ipv6_src_unk_action = l3_vp_port_tbl.bf.ipv6_src_unk_cmd;

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_intf_get(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry)
{
	sw_error_t rv = SW_OK;
	union in_l3_if_tbl_u in_l3_if_tbl;
	union eg_l3_if_tbl_u eg_l3_if_tbl;

	memset(&in_l3_if_tbl, 0, sizeof(in_l3_if_tbl));
	memset(&eg_l3_if_tbl, 0, sizeof(eg_l3_if_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	rv = hppe_in_l3_if_tbl_get(dev_id, index, &in_l3_if_tbl);
	if( rv != SW_OK )
		return rv;
	rv = hppe_eg_l3_if_tbl_get(dev_id, index, &eg_l3_if_tbl);
	if( rv != SW_OK )
		return rv;

	entry->mru = in_l3_if_tbl.bf.mru;
	entry->mtu = in_l3_if_tbl.bf.mtu;
	entry->ttl_dec_bypass_en = in_l3_if_tbl.bf.ttl_dec_bypass;
	entry->ipv4_uc_route_en = in_l3_if_tbl.bf.ipv4_uc_route_en;
	entry->ipv6_uc_route_en = in_l3_if_tbl.bf.ipv6_uc_route_en;
	entry->icmp_trigger_en = in_l3_if_tbl.bf.icmp_trigger_en;
	entry->ttl_exceed_action = in_l3_if_tbl.bf.ttl_exceed_cmd;
	entry->ttl_exceed_deacclr_en = in_l3_if_tbl.bf.ttl_exceed_de_acce;
	entry->mac_addr_bitmap = in_l3_if_tbl.bf.mac_bitmap;
	entry->mac_addr.uc[5] = eg_l3_if_tbl.bf.mac_addr_0;
	entry->mac_addr.uc[4] = eg_l3_if_tbl.bf.mac_addr_0 >> 8;
	entry->mac_addr.uc[3] = eg_l3_if_tbl.bf.mac_addr_0 >> 16;
	entry->mac_addr.uc[2] = eg_l3_if_tbl.bf.mac_addr_0 >> 24;
	entry->mac_addr.uc[1] = eg_l3_if_tbl.bf.mac_addr_1;
	entry->mac_addr.uc[0] = eg_l3_if_tbl.bf.mac_addr_1 >> 8;

	if (rv == SW_OK) {
		union rt_interface_cnt_tbl_u cnt_ingress, cnt_egress;
		hppe_rt_interface_cnt_tbl_get(dev_id, index, &cnt_ingress);
		hppe_rt_interface_cnt_tbl_get(dev_id, index + 256, &cnt_egress);
		entry->counter.rx_pkt_counter = cnt_ingress.bf.pkt_cnt;
		entry->counter.rx_byte_counter = cnt_ingress.bf.byte_cnt_0 | ((a_uint64_t)cnt_ingress.bf.byte_cnt_1 << 32);
		entry->counter.rx_drop_pkt_counter = cnt_ingress.bf.drop_pkt_cnt_0 | (cnt_ingress.bf.drop_pkt_cnt_1 << 24);
		entry->counter.rx_drop_byte_counter = cnt_ingress.bf.drop_byte_cnt_0 | \
								((a_uint64_t)cnt_ingress.bf.drop_byte_cnt_1 << 24);

		entry->counter.tx_pkt_counter = cnt_egress.bf.pkt_cnt;
		entry->counter.tx_byte_counter = cnt_egress.bf.byte_cnt_0 | ((a_uint64_t)cnt_egress.bf.byte_cnt_1 << 32);
		entry->counter.tx_drop_pkt_counter = cnt_egress.bf.drop_pkt_cnt_0 | (cnt_egress.bf.drop_pkt_cnt_1 << 24);
		entry->counter.tx_drop_byte_counter = cnt_egress.bf.drop_byte_cnt_0 | \
								((a_uint64_t)cnt_egress.bf.drop_byte_cnt_1 << 24);
	}

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_pub_addr_set(a_uint32_t dev_id,
		a_uint32_t index, fal_ip_pub_addr_t *entry)
{
	union in_pub_ip_addr_tbl_u in_pub_ip_addr_tbl;

	memset(&in_pub_ip_addr_tbl, 0, sizeof(in_pub_ip_addr_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	in_pub_ip_addr_tbl.bf.ip_addr = entry->pub_ip_addr;
	return hppe_in_pub_ip_addr_tbl_set(dev_id, index, &in_pub_ip_addr_tbl);
}

sw_error_t
adpt_hppe_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode,
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
		return hppe_host_flush_common(dev_id);
	}

	if ((type & FAL_IP_IP4_ADDR) == FAL_IP_IP4_ADDR) {
		union host_tbl_u entry;
		entry.bf.valid= 1;
		entry.bf.key_type = 0;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->dst_info;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ip_addr = host_entry->ip4_addr;
		rv = hppe_host_ipv4_del(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	} else if ((type & FAL_IP_IP6_ADDR) == FAL_IP_IP6_ADDR) {
		union host_ipv6_tbl_u entry;
		entry.bf.valid= 1;
		entry.bf.key_type = 2;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->dst_info;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.ipv6_addr_0 = host_entry->ip6_addr.ul[3];
		entry.bf.ipv6_addr_1 = host_entry->ip6_addr.ul[3] >> 10 | \
							host_entry->ip6_addr.ul[2] << 22;
		entry.bf.ipv6_addr_2 = host_entry->ip6_addr.ul[2] >> 10 | \
							host_entry->ip6_addr.ul[1] << 22;
		entry.bf.ipv6_addr_3 = host_entry->ip6_addr.ul[1] >> 10 | \
							host_entry->ip6_addr.ul[0] << 22;
		entry.bf.ipv6_addr_4 = host_entry->ip6_addr.ul[0] >> 10;
		rv = hppe_host_ipv6_del(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	} else if ((type & FAL_IP_IP4_ADDR_MCAST) == FAL_IP_IP4_ADDR_MCAST) {
		union host_ipv4_mcast_tbl_u entry;
		entry.bf.valid= 1;
		entry.bf.key_type = 1;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->dst_info;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.gip_addr_0 = host_entry->ip4_addr;
		entry.bf.gip_addr_1 = host_entry->ip4_addr >> 11;
		entry.bf.vsi = host_entry->mcast_info.vsi;
		entry.bf.sip_addr_0 = host_entry->mcast_info.sip4_addr;
		entry.bf.sip_addr_1 = host_entry->mcast_info.sip4_addr >> 11;
		rv = hppe_host_ipv4_mcast_del(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	} else if ((type & FAL_IP_IP6_ADDR_MCAST) == FAL_IP_IP6_ADDR_MCAST) {
		union host_ipv6_mcast_tbl_u entry;
		entry.bf.valid= 1;
		entry.bf.key_type = 3;
		entry.bf.fwd_cmd = host_entry->action;
		entry.bf.syn_toggle = host_entry->syn_toggle;
		entry.bf.dst_info = host_entry->dst_info;
		entry.bf.lan_wan = host_entry->lan_wan;
		entry.bf.gipv6_addr_0 = host_entry->ip6_addr.ul[3];
		entry.bf.gipv6_addr_1 = host_entry->ip6_addr.ul[3] >> 20 | \
							host_entry->ip6_addr.ul[2] << 12;
		entry.bf.gipv6_addr_2 = host_entry->ip6_addr.ul[2] >> 20 | \
							host_entry->ip6_addr.ul[1] << 12;
		entry.bf.gipv6_addr_3 = host_entry->ip6_addr.ul[1] >> 20 | \
							host_entry->ip6_addr.ul[0] << 12;
		entry.bf.gipv6_addr_4 = host_entry->ip6_addr.ul[0] >> 20;
		entry.bf.vsi = host_entry->mcast_info.vsi;
		entry.bf.sipv6_addr_0 = host_entry->mcast_info.sip6_addr.ul[3];
		entry.bf.sipv6_addr_1 = host_entry->mcast_info.sip6_addr.ul[3] >> 20 | \
							host_entry->mcast_info.sip6_addr.ul[2] << 12;
		entry.bf.sipv6_addr_2 = host_entry->mcast_info.sip6_addr.ul[2] >> 20 | \
							host_entry->mcast_info.sip6_addr.ul[1] << 12;
		entry.bf.sipv6_addr_3 = host_entry->mcast_info.sip6_addr.ul[1] >> 20 | \
							host_entry->mcast_info.sip6_addr.ul[0] << 12;
		entry.bf.sipv6_addr_4 = host_entry->mcast_info.sip6_addr.ul[0] >> 20;
		rv = hppe_host_ipv6_mcast_del(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
	}
	return rv;
}
sw_error_t
adpt_hppe_ip_route_mismatch_get(a_uint32_t dev_id, fal_fwd_cmd_t *cmd)
{
	sw_error_t rv = SW_OK;
	union l3_route_ctrl_ext_u l3_route_ctrl_ext;

	memset(&l3_route_ctrl_ext, 0, sizeof(l3_route_ctrl_ext));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cmd);

	rv = hppe_l3_route_ctrl_ext_get(dev_id, &l3_route_ctrl_ext);
	if( rv != SW_OK )
		return rv;

	*cmd = (fal_fwd_cmd_t)l3_route_ctrl_ext.bf.ip_route_mismatch;
	return rv;
}

sw_error_t
adpt_hppe_ip_vsi_arp_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
	sw_error_t rv = SW_OK;
	union l3_vsi_ext_u l3_vsi_ext;

	memset(&l3_vsi_ext, 0, sizeof(l3_vsi_ext));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(arp_sg_cfg);

	rv = hppe_l3_vsi_ext_get(dev_id, vsi, &l3_vsi_ext);
	if( rv != SW_OK )
		return rv;

	arp_sg_cfg->ipv4_arp_sg_en = l3_vsi_ext.bf.ip_arp_sg_en;
	arp_sg_cfg->ipv4_arp_sg_vio_action = l3_vsi_ext.bf.ip_arp_sg_vio_cmd;
	arp_sg_cfg->ipv4_arp_sg_port_en = l3_vsi_ext.bf.ip_arp_sg_port_en;
	arp_sg_cfg->ipv4_arp_sg_svlan_en = l3_vsi_ext.bf.ip_arp_sg_svlan_en;
	arp_sg_cfg->ipv4_arp_sg_cvlan_en = l3_vsi_ext.bf.ip_arp_sg_cvlan_en;
	arp_sg_cfg->ipv4_arp_src_unk_action = l3_vsi_ext.bf.ip_arp_src_unk_cmd;
	arp_sg_cfg->ip_nd_sg_en = l3_vsi_ext.bf.ip_nd_sg_en;
	arp_sg_cfg->ip_nd_sg_vio_action = l3_vsi_ext.bf.ip_nd_sg_vio_cmd;
	arp_sg_cfg->ip_nd_sg_port_en = l3_vsi_ext.bf.ip_nd_sg_port_en;
	arp_sg_cfg->ip_nd_sg_svlan_en = l3_vsi_ext.bf.ip_nd_sg_svlan_en;
	arp_sg_cfg->ip_nd_sg_cvlan_en = l3_vsi_ext.bf.ip_nd_sg_cvlan_en;
	arp_sg_cfg->ip_nd_src_unk_action = l3_vsi_ext.bf.ip_nd_src_unk_cmd;

	return rv;
}

sw_error_t
adpt_hppe_ip_port_arp_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
	union l3_vp_port_tbl_u l3_vp_port_tbl;

	memset(&l3_vp_port_tbl, 0, sizeof(l3_vp_port_tbl));
	ADPT_DEV_ID_CHECK(dev_id);

	hppe_l3_vp_port_tbl_get(dev_id, port_id, &l3_vp_port_tbl);

	l3_vp_port_tbl.bf.ip_arp_sg_en = arp_sg_cfg->ipv4_arp_sg_en;
	l3_vp_port_tbl.bf.ip_arp_sg_vio_cmd = arp_sg_cfg->ipv4_arp_sg_vio_action;
	l3_vp_port_tbl.bf.ip_arp_sg_port_en = arp_sg_cfg->ipv4_arp_sg_port_en;
	l3_vp_port_tbl.bf.ip_arp_sg_svlan_en = arp_sg_cfg->ipv4_arp_sg_svlan_en;
	l3_vp_port_tbl.bf.ip_arp_sg_cvlan_en = arp_sg_cfg->ipv4_arp_sg_cvlan_en;
	l3_vp_port_tbl.bf.ip_arp_src_unk_cmd = arp_sg_cfg->ipv4_arp_src_unk_action;
	l3_vp_port_tbl.bf.ip_nd_sg_en = arp_sg_cfg->ip_nd_sg_en;
	l3_vp_port_tbl.bf.ip_nd_sg_vio_cmd = arp_sg_cfg->ip_nd_sg_vio_action;
	l3_vp_port_tbl.bf.ip_nd_sg_port_en = arp_sg_cfg->ip_nd_sg_port_en;
	l3_vp_port_tbl.bf.ip_nd_sg_svlan_en = arp_sg_cfg->ip_nd_sg_svlan_en;
	l3_vp_port_tbl.bf.ip_nd_sg_cvlan_en = arp_sg_cfg->ip_nd_sg_cvlan_en;
	l3_vp_port_tbl.bf.ip_nd_src_unk_cmd = arp_sg_cfg->ip_nd_src_unk_action;
	
	return hppe_l3_vp_port_tbl_set(dev_id, port_id, &l3_vp_port_tbl);
}
sw_error_t
adpt_hppe_ip_vsi_mc_mode_set(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg)
{
	union l3_vsi_u l3_vsi;

	memset(&l3_vsi, 0, sizeof(l3_vsi));
	ADPT_DEV_ID_CHECK(dev_id);

	hppe_l3_vsi_get(dev_id, vsi, &l3_vsi);

	l3_vsi.bf.l2_ipv4_mc_en = cfg->l2_ipv4_mc_en;
	l3_vsi.bf.l2_ipv4_mc_mode = cfg->l2_ipv4_mc_mode;
	l3_vsi.bf.l2_ipv6_mc_en = cfg->l2_ipv6_mc_en;
	l3_vsi.bf.l2_ipv6_mc_mode = cfg->l2_ipv6_mc_mode;
	
	return hppe_l3_vsi_set(dev_id, vsi, &l3_vsi);
}

sw_error_t
adpt_hppe_ip_vsi_intf_set(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id)
{
	union l3_vsi_u l3_vsi;

	memset(&l3_vsi, 0, sizeof(l3_vsi));
	ADPT_DEV_ID_CHECK(dev_id);

	hppe_l3_vsi_get(dev_id, vsi, &l3_vsi);

	l3_vsi.bf.l3_if_valid = id->l3_if_valid;
	l3_vsi.bf.l3_if_index = id->l3_if_index;
	
	return hppe_l3_vsi_set(dev_id, vsi, &l3_vsi);
}

sw_error_t
adpt_hppe_ip_nexthop_get(a_uint32_t dev_id,
		a_uint32_t index, fal_ip_nexthop_t *entry)
{
	sw_error_t rv = SW_OK;
	union in_nexthop_tbl_u in_nexthop_tbl;

	memset(&in_nexthop_tbl, 0, sizeof(in_nexthop_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	rv = hppe_in_nexthop_tbl_get(dev_id, index, &in_nexthop_tbl);
	if( rv != SW_OK )
		return rv;

	entry->type = in_nexthop_tbl.bf0.type;
	entry->vsi = in_nexthop_tbl.bf1.vsi;
	entry->port = in_nexthop_tbl.bf0.port;
	entry->if_index = in_nexthop_tbl.bf0.post_l3_if;
	entry->ip_to_me_en = in_nexthop_tbl.bf0.ip_to_me;
	entry->pub_ip_index = in_nexthop_tbl.bf1.ip_pub_addr_index;
	entry->stag_fmt = in_nexthop_tbl.bf0.stag_fmt;
	entry->svid = in_nexthop_tbl.bf0.svid;
	entry->ctag_fmt = in_nexthop_tbl.bf0.ctag_fmt;
	entry->cvid = in_nexthop_tbl.bf0.cvid;
	entry->mac_addr.uc[5] = in_nexthop_tbl.bf1.mac_addr_0;
	entry->mac_addr.uc[4] = in_nexthop_tbl.bf1.mac_addr_0 >> 8;
	entry->mac_addr.uc[3] = in_nexthop_tbl.bf1.mac_addr_1;
	entry->mac_addr.uc[2] = in_nexthop_tbl.bf1.mac_addr_1 >> 8;
	entry->mac_addr.uc[1] = in_nexthop_tbl.bf1.mac_addr_1 >> 16;
	entry->mac_addr.uc[0] = in_nexthop_tbl.bf1.mac_addr_1 >> 24;
	entry->dnat_ip = in_nexthop_tbl.bf0.ip_addr_dnat;

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_route_mismatch_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
	union l3_route_ctrl_ext_u l3_route_ctrl_ext;

	memset(&l3_route_ctrl_ext, 0, sizeof(l3_route_ctrl_ext));
	ADPT_DEV_ID_CHECK(dev_id);

	hppe_l3_route_ctrl_ext_get(dev_id, &l3_route_ctrl_ext);
	l3_route_ctrl_ext.bf.ip_route_mismatch = cmd;
	
	return hppe_l3_route_ctrl_ext_set(dev_id, &l3_route_ctrl_ext);
}
sw_error_t
adpt_hppe_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode,
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
		rv = hppe_host_ipv4_get(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
		if (!rv && (entry.bf.key_type != 0))
			rv = SW_FAIL;
		host_entry->ip4_addr = entry.bf.ip_addr;
		host_entry->lan_wan = entry.bf.lan_wan;
		host_entry->dst_info = entry.bf.dst_info;
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
		rv = hppe_host_ipv6_get(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
		if (!rv && (entry.bf.key_type != 2))
			rv = SW_FAIL;
		host_entry->ip6_addr.ul[3] = entry.bf.ipv6_addr_0 | entry.bf.ipv6_addr_1 << 10;
		host_entry->ip6_addr.ul[2] = entry.bf.ipv6_addr_1 >> 22 | entry.bf.ipv6_addr_2 << 10;
		host_entry->ip6_addr.ul[1] = entry.bf.ipv6_addr_2 >> 22 | entry.bf.ipv6_addr_3 << 10;
		host_entry->ip6_addr.ul[0] = entry.bf.ipv6_addr_3 >> 22 | entry.bf.ipv6_addr_4 << 10;
		host_entry->lan_wan = entry.bf.lan_wan;
		host_entry->dst_info = entry.bf.dst_info;
		host_entry->syn_toggle = entry.bf.syn_toggle;
		host_entry->action = entry.bf.fwd_cmd;
		host_entry->status = entry.bf.valid;
	} else if ((type & FAL_IP_IP4_ADDR_MCAST) == FAL_IP_IP4_ADDR_MCAST) {
		union host_ipv4_mcast_tbl_u entry;
		entry.bf.key_type = 1;
		entry.bf.gip_addr_0 = host_entry->ip4_addr;
		entry.bf.gip_addr_1 = host_entry->ip4_addr >> 11;
		entry.bf.vsi = host_entry->mcast_info.vsi;
		entry.bf.sip_addr_0 = host_entry->mcast_info.sip4_addr;
		entry.bf.sip_addr_1 = host_entry->mcast_info.sip4_addr >> 11;
		rv = hppe_host_ipv4_mcast_get(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
		if (!rv && (entry.bf.key_type != 1))
			rv = SW_FAIL;
		host_entry->lan_wan = entry.bf.lan_wan;
		host_entry->dst_info = entry.bf.dst_info;
		host_entry->syn_toggle = entry.bf.syn_toggle;
		host_entry->action = entry.bf.fwd_cmd;
		host_entry->status = entry.bf.valid;
		host_entry->ip4_addr = entry.bf.gip_addr_0 | entry.bf.gip_addr_1 << 11;
		host_entry->mcast_info.vsi = entry.bf.vsi;
		host_entry->mcast_info.sip4_addr = entry.bf.sip_addr_0 | entry.bf.sip_addr_1 << 11;
	} else if ((type & FAL_IP_IP6_ADDR_MCAST) == FAL_IP_IP6_ADDR_MCAST) {
		union host_ipv6_mcast_tbl_u entry;
		entry.bf.key_type = 3;
		entry.bf.vsi = host_entry->mcast_info.vsi;
		entry.bf.gipv6_addr_0 = host_entry->ip6_addr.ul[3];
		entry.bf.gipv6_addr_1 = host_entry->ip6_addr.ul[3] >> 20 | \
							host_entry->ip6_addr.ul[2] << 12;
		entry.bf.gipv6_addr_2 = host_entry->ip6_addr.ul[2] >> 20 | \
							host_entry->ip6_addr.ul[1] << 12;
		entry.bf.gipv6_addr_3 = host_entry->ip6_addr.ul[1] >> 20 | \
							host_entry->ip6_addr.ul[0] << 12;
		entry.bf.gipv6_addr_4 = host_entry->ip6_addr.ul[0] >> 20;
		entry.bf.sipv6_addr_0 = host_entry->mcast_info.sip6_addr.ul[3];
		entry.bf.sipv6_addr_1 = host_entry->mcast_info.sip6_addr.ul[3] >> 20 | \
							host_entry->mcast_info.sip6_addr.ul[2] << 12;
		entry.bf.sipv6_addr_2 = host_entry->mcast_info.sip6_addr.ul[2] >> 20 | \
							host_entry->mcast_info.sip6_addr.ul[1] << 12;
		entry.bf.sipv6_addr_3 = host_entry->mcast_info.sip6_addr.ul[1] >> 20 | \
							host_entry->mcast_info.sip6_addr.ul[0] << 12;
		entry.bf.sipv6_addr_4 = host_entry->mcast_info.sip6_addr.ul[0] >> 20;
		rv = hppe_host_ipv6_mcast_get(dev_id, (a_uint32_t)mode, &host_entry->entry_id, &entry);
		if (!rv && (entry.bf.key_type != 3))
			rv = SW_FAIL;
		host_entry->lan_wan = entry.bf.lan_wan;
		host_entry->dst_info = entry.bf.dst_info;
		host_entry->syn_toggle = entry.bf.syn_toggle;
		host_entry->action = entry.bf.fwd_cmd;
		host_entry->status = entry.bf.valid;
		host_entry->ip6_addr.ul[3] = entry.bf.gipv6_addr_0 | entry.bf.gipv6_addr_1 << 20;
		host_entry->ip6_addr.ul[2] = entry.bf.gipv6_addr_1 >> 12 | entry.bf.gipv6_addr_2 << 20;
		host_entry->ip6_addr.ul[1] = entry.bf.gipv6_addr_2 >> 12 | entry.bf.gipv6_addr_3 << 20;
		host_entry->ip6_addr.ul[0] = entry.bf.gipv6_addr_3 >> 12 | entry.bf.gipv6_addr_4 << 20;
		host_entry->mcast_info.vsi = entry.bf.vsi;
		host_entry->mcast_info.sip6_addr.ul[3] = entry.bf.sipv6_addr_0 | entry.bf.sipv6_addr_1 << 20;
		host_entry->mcast_info.sip6_addr.ul[2] = entry.bf.sipv6_addr_1 >> 12 | entry.bf.sipv6_addr_2 << 20;
		host_entry->mcast_info.sip6_addr.ul[1] = entry.bf.sipv6_addr_2 >> 12 | entry.bf.sipv6_addr_3 << 20;
		host_entry->mcast_info.sip6_addr.ul[0] = entry.bf.sipv6_addr_3 >> 12 | entry.bf.sipv6_addr_4 << 20;
	}

	return rv;
}

sw_error_t
adpt_hppe_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode,
                     fal_host_entry_t * host_entry)
{
	a_uint32_t i = 0, step = 0;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(host_entry);

	if (FAL_NEXT_ENTRY_FIRST_ID == host_entry->entry_id)
		i = 0;

	if (next_mode == FAL_IP_IP6_ADDR_MCAST) {
		if (FAL_NEXT_ENTRY_FIRST_ID != host_entry->entry_id)
			i = (host_entry->entry_id & ~3) + 4;
		step = 4;
	} else if (next_mode == FAL_IP_IP4_ADDR_MCAST) {
		if (FAL_NEXT_ENTRY_FIRST_ID != host_entry->entry_id)
			i = (host_entry->entry_id & ~1) + 2;
		step = 2;
	} else if (next_mode == FAL_IP_IP4_ADDR) {
		if (FAL_NEXT_ENTRY_FIRST_ID != host_entry->entry_id)
			i = host_entry->entry_id + 1;
		step = 1;
	} else if (next_mode == FAL_IP_IP6_ADDR) {
		if (FAL_NEXT_ENTRY_FIRST_ID != host_entry->entry_id)
			i = (host_entry->entry_id & ~1) + 2;
		step = 2;
	}
	for (; i < HOST_TBL_MAX_ENTRY;) {
		host_entry->flags = next_mode;
		host_entry->entry_id = i;
		rv = adpt_hppe_ip_host_get(dev_id, FAL_IP_ENTRY_ID_EN, host_entry);
		if (!rv) {
			return rv;
		}
		i += step;
	}

	return SW_FAIL;

}
sw_error_t
adpt_hppe_ip_intf_set(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry)
{
	union in_l3_if_tbl_u in_l3_if_tbl;
	union eg_l3_if_tbl_u eg_l3_if_tbl;
	a_uint8_t i = 0;

	memset(&in_l3_if_tbl, 0, sizeof(in_l3_if_tbl));
	memset(&eg_l3_if_tbl, 0, sizeof(eg_l3_if_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	in_l3_if_tbl.bf.mru = entry->mru;
	in_l3_if_tbl.bf.mtu = entry->mtu;
	in_l3_if_tbl.bf.ttl_dec_bypass = entry->ttl_dec_bypass_en;
	in_l3_if_tbl.bf.ipv4_uc_route_en = entry->ipv4_uc_route_en;
	in_l3_if_tbl.bf.ipv6_uc_route_en = entry->ipv6_uc_route_en;
	in_l3_if_tbl.bf.icmp_trigger_en = entry->icmp_trigger_en;
	in_l3_if_tbl.bf.ttl_exceed_cmd = entry->ttl_exceed_action;
	in_l3_if_tbl.bf.ttl_exceed_de_acce = entry->ttl_exceed_deacclr_en;
	in_l3_if_tbl.bf.mac_bitmap = entry->mac_addr_bitmap;
	eg_l3_if_tbl.bf.mac_addr_0 = entry->mac_addr.uc[5] | \
							entry->mac_addr.uc[4] << 8 | \
							entry->mac_addr.uc[3] << 16 | \
							entry->mac_addr.uc[2] << 24;
	eg_l3_if_tbl.bf.mac_addr_1 = entry->mac_addr.uc[1] | \
							entry->mac_addr.uc[0] << 8;

	for (i = 0; i < 8; i++) {
		if ((entry->mac_addr_bitmap >> i) & 0x1) {
			union my_mac_tbl_u mymac;
			mymac.bf.valid = 1;
			mymac.bf.mac_da_0 = eg_l3_if_tbl.bf.mac_addr_0;
			mymac.bf.mac_da_1 = eg_l3_if_tbl.bf.mac_addr_1;
			hppe_my_mac_tbl_set(dev_id, i, &mymac);
			break;
		}
	}
	
	hppe_in_l3_if_tbl_set(dev_id, index, &in_l3_if_tbl);
	hppe_eg_l3_if_tbl_set(dev_id, index, &eg_l3_if_tbl);
	return SW_OK;
}

sw_error_t
adpt_hppe_ip_vsi_mc_mode_get(a_uint32_t dev_id, a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;
	union l3_vsi_u l3_vsi;

	memset(&l3_vsi, 0, sizeof(l3_vsi));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cfg);

	rv = hppe_l3_vsi_get(dev_id, vsi, &l3_vsi);
	if( rv != SW_OK )
		return rv;

	cfg->l2_ipv4_mc_en = l3_vsi.bf.l2_ipv4_mc_en;
	cfg->l2_ipv4_mc_mode = l3_vsi.bf.l2_ipv4_mc_mode;
	cfg->l2_ipv6_mc_en = l3_vsi.bf.l2_ipv6_mc_en;
	cfg->l2_ipv6_mc_mode = l3_vsi.bf.l2_ipv6_mc_mode;

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_port_macaddr_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr)
{
	sw_error_t rv = SW_OK;
	union l3_vp_port_tbl_u l3_vp_port_tbl;

	memset(&l3_vp_port_tbl, 0, sizeof(l3_vp_port_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(macaddr);

	rv = hppe_l3_vp_port_tbl_get(dev_id, port_id, &l3_vp_port_tbl);
	if( rv != SW_OK )
		return rv;

	macaddr->valid = l3_vp_port_tbl.bf.mac_valid;
	macaddr->mac_addr.uc[5] = l3_vp_port_tbl.bf.mac_da_0;
	macaddr->mac_addr.uc[4] = l3_vp_port_tbl.bf.mac_da_0 >> 8;
	macaddr->mac_addr.uc[3] = l3_vp_port_tbl.bf.mac_da_1;
	macaddr->mac_addr.uc[2] = l3_vp_port_tbl.bf.mac_da_1 >> 8;
	macaddr->mac_addr.uc[1] = l3_vp_port_tbl.bf.mac_da_1 >> 16;
	macaddr->mac_addr.uc[0] = l3_vp_port_tbl.bf.mac_da_1 >> 24;

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_port_arp_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg)
{
	sw_error_t rv = SW_OK;
	union l3_vp_port_tbl_u l3_vp_port_tbl;

	memset(&l3_vp_port_tbl, 0, sizeof(l3_vp_port_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(arp_sg_cfg);

	rv = hppe_l3_vp_port_tbl_get(dev_id, port_id, &l3_vp_port_tbl);
	if( rv != SW_OK )
		return rv;

	arp_sg_cfg->ipv4_arp_sg_en = l3_vp_port_tbl.bf.ip_arp_sg_en;
	arp_sg_cfg->ipv4_arp_sg_vio_action = l3_vp_port_tbl.bf.ip_arp_sg_vio_cmd;
	arp_sg_cfg->ipv4_arp_sg_port_en = l3_vp_port_tbl.bf.ip_arp_sg_port_en;
	arp_sg_cfg->ipv4_arp_sg_svlan_en = l3_vp_port_tbl.bf.ip_arp_sg_svlan_en;
	arp_sg_cfg->ipv4_arp_sg_cvlan_en = l3_vp_port_tbl.bf.ip_arp_sg_cvlan_en;
	arp_sg_cfg->ipv4_arp_src_unk_action = l3_vp_port_tbl.bf.ip_arp_src_unk_cmd;
	arp_sg_cfg->ip_nd_sg_en = l3_vp_port_tbl.bf.ip_nd_sg_en;
	arp_sg_cfg->ip_nd_sg_vio_action = l3_vp_port_tbl.bf.ip_nd_sg_vio_cmd;
	arp_sg_cfg->ip_nd_sg_port_en = l3_vp_port_tbl.bf.ip_nd_sg_port_en;
	arp_sg_cfg->ip_nd_sg_svlan_en = l3_vp_port_tbl.bf.ip_nd_sg_svlan_en;
	arp_sg_cfg->ip_nd_sg_cvlan_en = l3_vp_port_tbl.bf.ip_nd_sg_cvlan_en;
	arp_sg_cfg->ip_nd_src_unk_action = l3_vp_port_tbl.bf.ip_nd_src_unk_cmd;

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_global_ctrl_get(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;
	union l3_route_ctrl_u l3_route_ctrl;
	union l3_route_ctrl_ext_u l3_route_ctrl_ext;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cfg);
	memset(&l3_route_ctrl, 0, sizeof(l3_route_ctrl));
	memset(&l3_route_ctrl, 0, sizeof(l3_route_ctrl_ext));

	rv = hppe_l3_route_ctrl_get(dev_id, &l3_route_ctrl);
	if( rv != SW_OK )
		return rv;

	rv = hppe_l3_route_ctrl_ext_get(dev_id, &l3_route_ctrl_ext);
	if( rv != SW_OK )
		return rv;

	cfg->mru_fail_action = l3_route_ctrl.bf.ip_mru_check_fail;
	cfg->mru_deacclr_en = l3_route_ctrl.bf.ip_mru_check_fail_de_acce;
	cfg->mtu_fail_action = l3_route_ctrl.bf.ip_mtu_fail;
	cfg->mtu_deacclr_en = l3_route_ctrl.bf.ip_mtu_fail_de_acce;
	cfg->mtu_nonfrag_fail_action = l3_route_ctrl.bf.ip_mtu_df_fail;
	cfg->mtu_df_deacclr_en = l3_route_ctrl.bf.ip_mtu_df_fail_de_acce;
	cfg->prefix_bc_action = l3_route_ctrl.bf.ip_prefix_bc_cmd;
	cfg->prefix_deacclr_en = l3_route_ctrl.bf.ip_prefix_bc_de_acce;
	cfg->icmp_rdt_action = l3_route_ctrl.bf.icmp_rdt_cmd;
	cfg->icmp_rdt_deacclr_en = l3_route_ctrl.bf.icmp_rdt_de_acce;
	cfg->hash_mode_0 = l3_route_ctrl_ext.bf.host_hash_mode_0;
	cfg->hash_mode_1 = l3_route_ctrl_ext.bf.host_hash_mode_1;

	return SW_OK;
}

sw_error_t
adpt_hppe_ip_global_ctrl_set(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;
	union l3_route_ctrl_u l3_route_ctrl;
	union l3_route_ctrl_ext_u l3_route_ctrl_ext;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cfg);
	memset(&l3_route_ctrl, 0, sizeof(l3_route_ctrl));
	memset(&l3_route_ctrl, 0, sizeof(l3_route_ctrl_ext));

	rv = hppe_l3_route_ctrl_get(dev_id, &l3_route_ctrl);
	if( rv != SW_OK )
		return rv;

	rv = hppe_l3_route_ctrl_ext_get(dev_id, &l3_route_ctrl_ext);
	if( rv != SW_OK )
		return rv;

	l3_route_ctrl.bf.ip_mru_check_fail = cfg->mru_fail_action;
	l3_route_ctrl.bf.ip_mru_check_fail_de_acce = cfg->mru_deacclr_en;
	l3_route_ctrl.bf.ip_mtu_fail = cfg->mtu_fail_action;
	l3_route_ctrl.bf.ip_mtu_fail_de_acce = cfg->mtu_deacclr_en;
	l3_route_ctrl.bf.ip_mtu_df_fail = cfg->mtu_nonfrag_fail_action;
	l3_route_ctrl.bf.ip_mtu_df_fail_de_acce = cfg->mtu_df_deacclr_en;
	l3_route_ctrl.bf.ip_prefix_bc_cmd =cfg->prefix_bc_action;
	l3_route_ctrl.bf.ip_prefix_bc_de_acce = cfg->prefix_deacclr_en;
	l3_route_ctrl.bf.icmp_rdt_cmd = cfg->icmp_rdt_action;
	l3_route_ctrl.bf.icmp_rdt_de_acce = cfg->icmp_rdt_deacclr_en;
	l3_route_ctrl_ext.bf.host_hash_mode_0 = cfg->hash_mode_0;
	l3_route_ctrl_ext.bf.host_hash_mode_1 = cfg->hash_mode_1;

	hppe_l3_route_ctrl_set(dev_id, &l3_route_ctrl);
	hppe_l3_route_ctrl_ext_set(dev_id, &l3_route_ctrl_ext);
	return SW_OK;
}

sw_error_t
adpt_hppe_ip_nexthop_set(a_uint32_t dev_id,
			a_uint32_t index, fal_ip_nexthop_t *entry)
{
	union in_nexthop_tbl_u in_nexthop_tbl;

	memset(&in_nexthop_tbl, 0, sizeof(in_nexthop_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	in_nexthop_tbl.bf0.type = entry->type;
	if (entry->type == 0)
		in_nexthop_tbl.bf1.vsi = entry->vsi;
	else
		in_nexthop_tbl.bf0.port = entry->port;
	in_nexthop_tbl.bf0.post_l3_if = entry->if_index;
	in_nexthop_tbl.bf0.ip_to_me = entry->ip_to_me_en;
	in_nexthop_tbl.bf0.ip_pub_addr_index = entry->pub_ip_index;
	in_nexthop_tbl.bf0.stag_fmt = entry->stag_fmt;
	in_nexthop_tbl.bf0.svid = entry->svid;
	in_nexthop_tbl.bf0.ctag_fmt = entry->ctag_fmt;
	in_nexthop_tbl.bf0.cvid = entry->cvid;
	in_nexthop_tbl.bf0.mac_addr_0 = entry->mac_addr.uc[5] |
					entry->mac_addr.uc[4] << 8;
	in_nexthop_tbl.bf0.mac_addr_1 = entry->mac_addr.uc[3] |
					entry->mac_addr.uc[2] << 8 |
					entry->mac_addr.uc[1] << 16 |
					entry->mac_addr.uc[0] << 24;
	in_nexthop_tbl.bf0.ip_addr_dnat = entry->dnat_ip;

	return hppe_in_nexthop_tbl_set(dev_id, index, &in_nexthop_tbl);
}
#endif
void adpt_hppe_ip_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_ip_func_bitmap[0] = 0;
	p_adpt_api->adpt_ip_func_bitmap[1] = 0;
	return;
}

static void adpt_hppe_ip_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_ip_network_route_get = NULL;
	p_adpt_api->adpt_ip_host_add = NULL;
	p_adpt_api->adpt_ip_vsi_sg_cfg_get = NULL;
	p_adpt_api->adpt_ip_pub_addr_set = NULL;
	p_adpt_api->adpt_ip_port_sg_cfg_set = NULL;
	p_adpt_api->adpt_ip_port_intf_get = NULL;
	p_adpt_api->adpt_ip_vsi_arp_sg_cfg_set = NULL;
	p_adpt_api->adpt_ip_pub_addr_get = NULL;
	p_adpt_api->adpt_ip_port_intf_set = NULL;
	p_adpt_api->adpt_ip_vsi_sg_cfg_set = NULL;
	p_adpt_api->adpt_ip_host_next = NULL;
	p_adpt_api->adpt_ip_port_macaddr_set = NULL;
	p_adpt_api->adpt_ip_vsi_intf_get = NULL;
	p_adpt_api->adpt_ip_network_route_add = NULL;
	p_adpt_api->adpt_ip_port_sg_cfg_get = NULL;
	p_adpt_api->adpt_ip_intf_get = NULL;
	p_adpt_api->adpt_ip_network_route_del = NULL;
	p_adpt_api->adpt_ip_host_del = NULL;
	p_adpt_api->adpt_ip_route_mismatch_get = NULL;
	p_adpt_api->adpt_ip_vsi_arp_sg_cfg_get = NULL;
	p_adpt_api->adpt_ip_port_arp_sg_cfg_set = NULL;
	p_adpt_api->adpt_ip_vsi_mc_mode_set = NULL;
	p_adpt_api->adpt_ip_vsi_intf_set = NULL;
	p_adpt_api->adpt_ip_nexthop_get = NULL;
	p_adpt_api->adpt_ip_route_mismatch_set = NULL;
	p_adpt_api->adpt_ip_host_get = NULL;
	p_adpt_api->adpt_ip_intf_set = NULL;
	p_adpt_api->adpt_ip_vsi_mc_mode_get = NULL;
	p_adpt_api->adpt_ip_port_macaddr_get = NULL;
	p_adpt_api->adpt_ip_port_arp_sg_cfg_get = NULL;
	p_adpt_api->adpt_ip_nexthop_set = NULL;
	p_adpt_api->adpt_ip_global_ctrl_get = NULL;
	p_adpt_api->adpt_ip_global_ctrl_set = NULL;

	return;
}
sw_error_t adpt_hppe_ip_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_ip_func_unregister(dev_id, p_adpt_api);
#ifndef IN_IP_MINI
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_NETWORK_ROUTE_GET))
		p_adpt_api->adpt_ip_network_route_get = adpt_hppe_ip_network_route_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_HOST_ADD))
		p_adpt_api->adpt_ip_host_add = adpt_hppe_ip_host_add;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_VSI_SG_CFG_GET))
		p_adpt_api->adpt_ip_vsi_sg_cfg_get = adpt_hppe_ip_vsi_sg_cfg_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PUB_ADDR_SET))
		p_adpt_api->adpt_ip_pub_addr_set = adpt_hppe_ip_pub_addr_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PORT_SG_CFG_SET))
		p_adpt_api->adpt_ip_port_sg_cfg_set = adpt_hppe_ip_port_sg_cfg_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PORT_INTF_GET))
		p_adpt_api->adpt_ip_port_intf_get = adpt_hppe_ip_port_intf_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_VSI_ARP_SG_CFG_SET))
		p_adpt_api->adpt_ip_vsi_arp_sg_cfg_set = adpt_hppe_ip_vsi_arp_sg_cfg_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PUB_ADDR_GET))
		p_adpt_api->adpt_ip_pub_addr_get = adpt_hppe_ip_pub_addr_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PORT_INTF_SET))
		p_adpt_api->adpt_ip_port_intf_set = adpt_hppe_ip_port_intf_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_VSI_SG_CFG_SET))
		p_adpt_api->adpt_ip_vsi_sg_cfg_set = adpt_hppe_ip_vsi_sg_cfg_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_HOST_NEXT))
		p_adpt_api->adpt_ip_host_next = adpt_hppe_ip_host_next;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PORT_MACADDR_SET))
		p_adpt_api->adpt_ip_port_macaddr_set = adpt_hppe_ip_port_macaddr_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_VSI_INTF_GET))
		p_adpt_api->adpt_ip_vsi_intf_get = adpt_hppe_ip_vsi_intf_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_NETWORK_ROUTE_ADD))
		p_adpt_api->adpt_ip_network_route_add = adpt_hppe_ip_network_route_add;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PORT_SG_CFG_GET))
		p_adpt_api->adpt_ip_port_sg_cfg_get = adpt_hppe_ip_port_sg_cfg_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_INTF_GET))
		p_adpt_api->adpt_ip_intf_get = adpt_hppe_ip_intf_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_NETWORK_ROUTE_DEL))
		p_adpt_api->adpt_ip_network_route_del = adpt_hppe_ip_network_route_del;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_HOST_DEL))
		p_adpt_api->adpt_ip_host_del = adpt_hppe_ip_host_del;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_ROUTE_MISMATCH_GET))
		p_adpt_api->adpt_ip_route_mismatch_get = adpt_hppe_ip_route_mismatch_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_VSI_ARP_SG_CFG_GET))
		p_adpt_api->adpt_ip_vsi_arp_sg_cfg_get = adpt_hppe_ip_vsi_arp_sg_cfg_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PORT_ARP_SG_CFG_SET))
		p_adpt_api->adpt_ip_port_arp_sg_cfg_set = adpt_hppe_ip_port_arp_sg_cfg_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_VSI_MC_MODE_SET))
		p_adpt_api->adpt_ip_vsi_mc_mode_set = adpt_hppe_ip_vsi_mc_mode_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_VSI_INTF_SET))
		p_adpt_api->adpt_ip_vsi_intf_set = adpt_hppe_ip_vsi_intf_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_NEXTHOP_GET))
		p_adpt_api->adpt_ip_nexthop_get = adpt_hppe_ip_nexthop_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_ROUTE_MISMATCH_SET))
		p_adpt_api->adpt_ip_route_mismatch_set = adpt_hppe_ip_route_mismatch_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_HOST_GET))
		p_adpt_api->adpt_ip_host_get = adpt_hppe_ip_host_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_INTF_SET))
		p_adpt_api->adpt_ip_intf_set = adpt_hppe_ip_intf_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_VSI_MC_MODE_GET))
		p_adpt_api->adpt_ip_vsi_mc_mode_get = adpt_hppe_ip_vsi_mc_mode_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PORT_MACADDR_GET))
		p_adpt_api->adpt_ip_port_macaddr_get = adpt_hppe_ip_port_macaddr_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_PORT_ARP_SG_CFG_GET))
		p_adpt_api->adpt_ip_port_arp_sg_cfg_get = adpt_hppe_ip_port_arp_sg_cfg_get;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_NEXTHOP_SET))
		p_adpt_api->adpt_ip_nexthop_set = adpt_hppe_ip_nexthop_set;
	if (p_adpt_api->adpt_ip_func_bitmap[0] & (1 << FUNC_IP_GLOBAL_CTRL_GET))
		p_adpt_api->adpt_ip_global_ctrl_get = adpt_hppe_ip_global_ctrl_get;
	if (p_adpt_api->adpt_ip_func_bitmap[1] & (1 << (FUNC_IP_GLOBAL_CTRL_SET % 32)))
		p_adpt_api->adpt_ip_global_ctrl_set = adpt_hppe_ip_global_ctrl_set;
#endif
	return SW_OK;
}
/**
 * @}
 */
