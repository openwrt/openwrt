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


/**
 * @defgroup fal_ip FAL_IP
 * @{
 */
#ifndef _FAL_IP_H_
#define _FAL_IP_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"
#include "fal_multi.h"

    /* IP WCMP hash key flags */
#define FAL_WCMP_HASH_KEY_SIP             0x1
#define FAL_WCMP_HASH_KEY_DIP             0x2
#define FAL_WCMP_HASH_KEY_SPORT           0x4
#define FAL_WCMP_HASH_KEY_DPORT           0x8

    /* IP entry operation flags */
#define FAL_IP_ENTRY_ID_EN               0x1
#define FAL_IP_ENTRY_INTF_EN             0x2
#define FAL_IP_ENTRY_PORT_EN             0x4
#define FAL_IP_ENTRY_STATUS_EN           0x8
#define FAL_IP_ENTRY_IPADDR_EN           0x10
#define FAL_IP_ENTRY_ALL_EN              0x20

    /* IP host entry structure flags field */
#define FAL_IP_IP4_ADDR                         0x1
#define FAL_IP_IP6_ADDR                         0x2
#define FAL_IP_CPU_ADDR                         0x4
#define FAL_IP_IP4_ADDR_MCAST              0x8
#define FAL_IP_IP6_ADDR_MCAST              0x10

typedef struct {
	a_uint8_t vsi; /*vsi value for l2 multicast*/
	fal_ip4_addr_t sip4_addr; /*source ipv4 address*/
	fal_ip6_addr_t sip6_addr; /*source ipv4 address*/
} fal_host_mcast_t;

typedef struct
{
	a_uint32_t rx_pkt_counter; /*rx packet counter*/
	a_uint64_t rx_byte_counter; /*rx byte counter*/
	a_uint32_t rx_drop_pkt_counter; /*rx drop packet counter*/
	a_uint64_t rx_drop_byte_counter; /*rx drop byte counter*/
	a_uint32_t tx_pkt_counter; /*tx packet counter*/
	a_uint64_t tx_byte_counter; /*tx byte counter*/
	a_uint32_t tx_drop_pkt_counter; /*tx drop packet counter*/
	a_uint64_t tx_drop_byte_counter; /*tx drop byte counter*/
} fal_ip_counter_t;

typedef struct
{
	a_uint32_t entry_id; /*index for host table*/
	a_uint32_t flags; /*1:ipv4 uni 2:ipv6 uni 8:ipv4 multi 0x10:ipv6 multi*/
	a_uint32_t status; /* valid status: 0 or 1*/
	fal_ip4_addr_t ip4_addr; /* ipv4 address */
	fal_ip6_addr_t ip6_addr; /* ipv6 address */
	fal_mac_addr_t mac_addr; /* unused for ppe */
	a_uint32_t intf_id; /* unused for ppe */
	a_uint32_t lb_num; /* unused for ppe */
	a_uint32_t vrf_id; /* unused for ppe */
	a_uint32_t expect_vid; /* unused for ppe */
	fal_port_t port_id; /* unused for ppe */
	a_bool_t mirror_en; /* unused for ppe */
	a_bool_t counter_en; /* unused for ppe */
	a_uint32_t counter_id; /* unused for ppe */
	a_uint32_t packet; /* unused for ppe */
	a_uint32_t byte; /* unused for ppe */
	a_bool_t pppoe_en; /* unused for ppe */
	a_uint32_t pppoe_id; /* unused for ppe */
	fal_fwd_cmd_t action; /*forward action*/
	a_uint32_t dst_info; /*bit 12:13: 1.nexthop, 2.port id, 3.port bitmap*/
	a_uint8_t syn_toggle; /* sync toggle */
	a_uint8_t lan_wan; /*0: ip over lan side ; 1: ip over wan side*/
	fal_host_mcast_t mcast_info; /* multicast information */
} fal_host_entry_t;

    typedef enum
    {
        FAL_MAC_IP_GUARD = 0,
        FAL_MAC_IP_PORT_GUARD,
        FAL_MAC_IP_VLAN_GUARD,
        FAL_MAC_IP_PORT_VLAN_GUARD,
        FAL_NO_SOURCE_GUARD,
    } fal_source_guard_mode_t;

    typedef enum
    {
        FAL_DEFAULT_FLOW_FORWARD = 0,
        FAL_DEFAULT_FLOW_DROP,
        FAL_DEFAULT_FLOW_RDT_TO_CPU,
        FAL_DEFAULT_FLOW_ADMIT_ALL,
    } fal_default_flow_cmd_t;

    typedef enum
    {
        FAL_FLOW_LAN_TO_LAN = 0,
        FAL_FLOW_WAN_TO_LAN,
        FAL_FLOW_LAN_TO_WAN,
        FAL_FLOW_WAN_TO_WAN,
    } fal_flow_type_t;

    typedef enum
    {
        FAL_ARP_LEARN_LOCAL = 0,
        FAL_ARP_LEARN_ALL,
    } fal_arp_learn_mode_t;

    /* IP host entry auto learn arp packets type */
#define FAL_ARP_LEARN_REQ                0x1
#define FAL_ARP_LEARN_ACK                0x2

    typedef struct
    {
        a_uint32_t entry_id;
        a_uint32_t vrf_id;
        a_uint16_t vid_low;
        a_uint16_t vid_high;
        fal_mac_addr_t mac_addr;
        a_bool_t ip4_route;
        a_bool_t ip6_route;
    } fal_intf_mac_entry_t;

    typedef struct
    {
        a_uint32_t nh_nr;
        a_uint32_t nh_id[16];
    } fal_ip_wcmp_t;

    typedef struct
    {
        fal_mac_addr_t mac_addr;
        fal_ip4_addr_t ip4_addr;
		a_uint32_t     vid;
		a_uint8_t      load_balance;
    } fal_ip4_rfs_t;

	typedef struct
    {
        fal_mac_addr_t mac_addr;
        fal_ip6_addr_t ip6_addr;
		a_uint32_t     vid;
		a_uint8_t      load_balance;
    } fal_ip6_rfs_t;

    typedef struct
    {
        a_bool_t valid;
        a_uint32_t vrf_id;
        fal_addr_type_t ip_version; /*0 for IPv4 and 1 for IPv6*/
        a_uint32_t droute_type; /*0 for ARP and 1 for WCMP*/
        a_uint32_t index;/*when droute_type equals 0, means ARP entry index or means WCMP indexs*/
    } fal_default_route_t;

    typedef struct
    {
        a_bool_t valid;
        a_uint32_t vrf_id;
        a_uint32_t ip_version; /*0 for IPv4 and 1 for IPv6*/
        union {
        fal_ip4_addr_t ip4_addr;
        fal_ip6_addr_t ip6_addr;
	}route_addr;
        a_uint32_t prefix_length;/*For IPv4, up to 32 and for IPv6, up to 128*/
    } fal_host_route_t;

typedef struct
{
	a_bool_t ipv4_arp_sg_en; /*0 for disable and 1 for enable*/
	fal_fwd_cmd_t ipv4_arp_sg_vio_action; /* check fail action for arp source guard */
	a_bool_t ipv4_arp_sg_port_en; /* source port based arp source guard enable */
	a_bool_t ipv4_arp_sg_svlan_en; /* source svlan based arp source guard enable */
	a_bool_t ipv4_arp_sg_cvlan_en; /* source cvlan based arp source guard enable */
	fal_fwd_cmd_t ipv4_arp_src_unk_action; /* unknown action for arp source guard */
	a_bool_t ip_nd_sg_en; /*0 for disable and 1 for enable*/
	fal_fwd_cmd_t ip_nd_sg_vio_action; /* check fail action for nd source guard */
	a_bool_t ip_nd_sg_port_en; /* source port based nd source guard enable */
	a_bool_t ip_nd_sg_svlan_en; /* source svlan based nd source guard enable */
	a_bool_t ip_nd_sg_cvlan_en; /* source cvlan based nd source guard enable */
	fal_fwd_cmd_t ip_nd_src_unk_action; /* unknown action for nd source guard */
} fal_arp_sg_cfg_t;

typedef enum
{
	FAL_MC_MODE_GV = 0, /*not support igmpv3 source filter*/
	FAL_MC_MODE_SGV /*support igmpv3 source filter*/
} fal_mc_mode_t;

typedef struct
{
	a_bool_t l2_ipv4_mc_en; /*0 for disable and 1 for enable*/
	fal_mc_mode_t l2_ipv4_mc_mode; /*two modes*/
	a_bool_t l2_ipv6_mc_en; /*0 for disable and 1 for enable*/
	fal_mc_mode_t l2_ipv6_mc_mode; /*same with IPv4*/
} fal_mc_mode_cfg_t;

typedef struct
{
	a_uint8_t type; /*0 for IPv4 and 1 for IPv6*/
	fal_fwd_cmd_t action; /* forward action */
	a_uint32_t dst_info; /*bit 12:13: 1.nexthop, 2.port id, 3.port bitmap*/
	a_uint8_t lan_wan; /* 0:ip over lan side; 1:ip over wan side */
	union {
		fal_ip4_addr_t ip4_addr; /* ipv4 address */
		fal_ip6_addr_t ip6_addr; /* ipv6 address */
	} route_addr;
	union {
		fal_ip4_addr_t ip4_addr_mask; /* ipv4 address mask */
		fal_ip6_addr_t ip6_addr_mask; /* ipv6 address mask */
	} route_addr_mask;
} fal_network_route_entry_t;

typedef struct {
	a_uint16_t mru; /* Maximum Receive Unit*/
	a_uint16_t mtu; /* Maximum Transmission Unit*/
	a_bool_t ttl_dec_bypass_en; /* Bypass TTL Decrement enable*/
	a_bool_t ipv4_uc_route_en; /*0 for disble and 1 for enable*/
	a_bool_t ipv6_uc_route_en; /*0 for disble and 1 for enable*/
	a_bool_t icmp_trigger_en; /* ICMP trigger flag enable*/
	fal_fwd_cmd_t ttl_exceed_action; /*action for ttl 0*/
	a_bool_t ttl_exceed_deacclr_en; /*0 for disble and 1 for enable*/
	a_uint8_t mac_addr_bitmap; /* bitmap for mac address*/
	fal_mac_addr_t mac_addr; /* mac address */
	fal_ip_counter_t counter; /* interface related counter */
} fal_intf_entry_t;

typedef struct
{
	a_bool_t l3_if_valid; /*0 for disable and 1 for enable*/
	a_uint32_t l3_if_index; /*index for interface table*/        	  
} fal_intf_id_t;

typedef enum
{
	FAL_NEXTHOP_L3 = 0,
	FAL_NEXTHOP_VP,
} fal_nexthop_type_t;

typedef struct
{
	fal_nexthop_type_t type; /* 0: L3 1:port*/
	a_uint8_t vsi; /* output vsi value if type is 0 */
	fal_port_t port; /* destination port */
	a_uint32_t if_index; /* egress interface index */
	a_bool_t ip_to_me_en; /* 0 for disable and 1 for enable*/
	a_uint8_t pub_ip_index; /*index to public ip address*/
	a_uint8_t stag_fmt; /* 0: untag 1:tagged*/
	a_uint16_t svid; /*svlan id*/
	a_int8_t ctag_fmt; /* 0: untag 1:tagged*/
	a_uint16_t cvid; /* cvlan id */
	fal_mac_addr_t mac_addr; /* mac address */
	fal_ip4_addr_t dnat_ip; /*dnat ip address*/
} fal_ip_nexthop_t;

typedef struct
{
	a_bool_t ipv4_sg_en; /*0 for disable and 1 for enable*/
	fal_fwd_cmd_t ipv4_sg_vio_action; /* check fail action for ipv4 source guard */
	a_bool_t ipv4_sg_port_en; /* source port based ipv4 source guard enable */
	a_bool_t ipv4_sg_svlan_en; /* source svlan based ipv4 source guard enable */
	a_bool_t ipv4_sg_cvlan_en; /* source cvlan based ipv4 source guard enable */
	fal_fwd_cmd_t ipv4_src_unk_action; /* unknown action for ipv4 source guard */
	a_bool_t ipv6_sg_en; /*0 for disable and 1 for enable*/
	fal_fwd_cmd_t ipv6_sg_vio_action; /* check fail action for ipv6 source guard */
	a_bool_t ipv6_sg_port_en; /* source port based ipv6 source guard enable */
	a_bool_t ipv6_sg_svlan_en; /* source svlan based ipv6 source guard enable */
	a_bool_t ipv6_sg_cvlan_en; /* source cvlan based ipv6 source guard enable */
	fal_fwd_cmd_t ipv6_src_unk_action; /* unknown action for ipv6 source guard */
} fal_sg_cfg_t;

typedef struct
{
	fal_ip4_addr_t pub_ip_addr; /*public ip address*/
} fal_ip_pub_addr_t;

typedef struct {
	a_bool_t valid; /* valid flag */
	fal_mac_addr_t mac_addr; /* mac address */
} fal_macaddr_entry_t;

typedef struct
{
	fal_fwd_cmd_t mru_fail_action; /*mru check fail action*/
	a_bool_t mru_deacclr_en; /*0 for disable and 1 for enable*/
	fal_fwd_cmd_t mtu_fail_action; /*mtu check fail action*/
	a_bool_t mtu_deacclr_en; /*0 for disable and 1 for enable*/
	fal_fwd_cmd_t mtu_nonfrag_fail_action; /*mtu check fail action for non-fragment */
	a_bool_t mtu_df_deacclr_en; /*0 for disable and 1 for enable*/
	fal_fwd_cmd_t prefix_bc_action; /*0 forward, 1 drop, 2 copy, 3 rdt_cpu*/
	a_bool_t prefix_deacclr_en; /*0 for disable and 1 for enable*/
	fal_fwd_cmd_t icmp_rdt_action; /*0 forward, 1 drop, 2 copy, 3 rdt_cpu*/
	a_bool_t icmp_rdt_deacclr_en; /*0 for disable and 1 for enable*/
	a_uint8_t hash_mode_0; /*0 crc10, 1 xor, 2 crc16*/
	a_uint8_t hash_mode_1; /*0 crc10, 1 xor, 2 crc16*/
} fal_ip_global_cfg_t;

enum {
	FUNC_IP_NETWORK_ROUTE_GET = 0,
	FUNC_IP_HOST_ADD,
	FUNC_IP_VSI_SG_CFG_GET,
	FUNC_IP_PUB_ADDR_SET,
	FUNC_IP_PORT_SG_CFG_SET,
	FUNC_IP_PORT_INTF_GET,
	FUNC_IP_VSI_ARP_SG_CFG_SET,
	FUNC_IP_PUB_ADDR_GET,
	FUNC_IP_PORT_INTF_SET,
	FUNC_IP_VSI_SG_CFG_SET,
	FUNC_IP_HOST_NEXT,
	FUNC_IP_PORT_MACADDR_SET,
	FUNC_IP_VSI_INTF_GET,
	FUNC_IP_NETWORK_ROUTE_ADD,
	FUNC_IP_PORT_SG_CFG_GET,
	FUNC_IP_INTF_GET,
	FUNC_IP_NETWORK_ROUTE_DEL,
	FUNC_IP_HOST_DEL,
	FUNC_IP_ROUTE_MISMATCH_GET,
	FUNC_IP_VSI_ARP_SG_CFG_GET,
	FUNC_IP_PORT_ARP_SG_CFG_SET,
	FUNC_IP_VSI_MC_MODE_SET,
	FUNC_IP_VSI_INTF_SET,
	FUNC_IP_NEXTHOP_GET,
	FUNC_IP_ROUTE_MISMATCH_SET,
	FUNC_IP_HOST_GET,
	FUNC_IP_INTF_SET,
	FUNC_IP_VSI_MC_MODE_GET,
	FUNC_IP_PORT_MACADDR_GET,
	FUNC_IP_PORT_ARP_SG_CFG_GET,
	FUNC_IP_NEXTHOP_SET,
	FUNC_IP_GLOBAL_CTRL_GET,
	FUNC_IP_GLOBAL_CTRL_SET,
};

    sw_error_t
    fal_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * host_entry);

    sw_error_t
    fal_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode,
                    fal_host_entry_t * host_entry);

    sw_error_t
    fal_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode,
                    fal_host_entry_t * host_entry);

    sw_error_t
    fal_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode,
                     fal_host_entry_t * host_entry);

    sw_error_t
    fal_ip_host_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                             a_uint32_t cnt_id, a_bool_t enable);

    sw_error_t
    fal_ip_host_pppoe_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                           a_uint32_t pppoe_id, a_bool_t enable);

    sw_error_t
    fal_ip_pt_arp_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t flags);

    sw_error_t
    fal_ip_pt_arp_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * flags);

    sw_error_t
    fal_ip_arp_learn_set(a_uint32_t dev_id, fal_arp_learn_mode_t mode);

    sw_error_t
    fal_ip_arp_learn_get(a_uint32_t dev_id, fal_arp_learn_mode_t * mode);

    sw_error_t
    fal_ip_source_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_source_guard_mode_t mode);

    sw_error_t
    fal_ip_source_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_source_guard_mode_t * mode);

    sw_error_t
    fal_ip_arp_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_source_guard_mode_t mode);

    sw_error_t
    fal_ip_arp_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_source_guard_mode_t * mode);

    sw_error_t
    fal_ip_route_status_set(a_uint32_t dev_id, a_bool_t enable);

    sw_error_t
    fal_ip_route_status_get(a_uint32_t dev_id, a_bool_t * enable);

    sw_error_t
    fal_ip_intf_entry_add(a_uint32_t dev_id, fal_intf_mac_entry_t * entry);

    sw_error_t
    fal_ip_intf_entry_del(a_uint32_t dev_id, a_uint32_t del_mode,
                          fal_intf_mac_entry_t * entry);

    sw_error_t
    fal_ip_intf_entry_next(a_uint32_t dev_id, a_uint32_t next_mode,
                           fal_intf_mac_entry_t * entry);

    sw_error_t
    fal_ip_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);

    sw_error_t
    fal_ip_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

    sw_error_t
    fal_arp_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);

    sw_error_t
    fal_arp_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

    sw_error_t
    fal_ip_age_time_set(a_uint32_t dev_id, a_uint32_t * time);

    sw_error_t
    fal_ip_age_time_get(a_uint32_t dev_id, a_uint32_t * time);

    sw_error_t
    fal_ip_wcmp_entry_set(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp);

    sw_error_t
    fal_ip_wcmp_entry_get(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp);

    sw_error_t
    fal_ip_wcmp_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode);

	sw_error_t
	fal_ip_rfs_ip4_rule_set(a_uint32_t dev_id, fal_ip4_rfs_t * rfs);

	sw_error_t
	fal_ip_rfs_ip6_rule_set(a_uint32_t dev_id, fal_ip6_rfs_t * rfs);

	sw_error_t
	fal_ip_rfs_ip4_rule_del(a_uint32_t dev_id, fal_ip4_rfs_t * rfs);

	sw_error_t
	fal_ip_rfs_ip6_rule_del(a_uint32_t dev_id, fal_ip6_rfs_t * rfs);

    sw_error_t
    fal_ip_wcmp_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode);

    sw_error_t
    fal_ip_vrf_base_addr_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t addr);

    sw_error_t
    fal_ip_vrf_base_addr_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t * addr);

    sw_error_t
    fal_ip_vrf_base_mask_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t addr);

    sw_error_t
    fal_ip_vrf_base_mask_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t * addr);

    sw_error_t
    fal_ip_default_route_set(a_uint32_t dev_id, a_uint32_t droute_id,
			fal_default_route_t * entry);

    sw_error_t
    fal_ip_default_route_get(a_uint32_t dev_id, a_uint32_t droute_id,
			fal_default_route_t * entry);

    sw_error_t
    fal_ip_host_route_set(a_uint32_t dev_id, a_uint32_t hroute_id,
			fal_host_route_t * entry);

    sw_error_t
    fal_ip_host_route_get(a_uint32_t dev_id, a_uint32_t hroute_id,
			fal_host_route_t * entry);

	sw_error_t
    fal_ip_wcmp_entry_set(a_uint32_t dev_id, a_uint32_t wcmp_id,
			fal_ip_wcmp_t * wcmp);

    sw_error_t
    fal_ip_wcmp_entry_get(a_uint32_t dev_id, a_uint32_t wcmp_id,
			fal_ip_wcmp_t * wcmp);

    sw_error_t
    fal_default_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t cmd);

    sw_error_t
    fal_default_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t * cmd);

    sw_error_t
    fal_default_rt_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t cmd);

    sw_error_t
    fal_default_rt_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id,
			fal_flow_type_t type, fal_default_flow_cmd_t * cmd);

sw_error_t
fal_ip_vsi_arp_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
			fal_arp_sg_cfg_t *arp_sg_cfg);

sw_error_t
fal_ip_vsi_arp_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
			fal_arp_sg_cfg_t *arp_sg_cfg);

sw_error_t
fal_ip_network_route_add(a_uint32_t dev_id, a_uint32_t index,
			fal_network_route_entry_t *entry);

sw_error_t
fal_ip_network_route_get(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type,
			fal_network_route_entry_t *entry);

sw_error_t
fal_ip_network_route_del(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type);

sw_error_t
fal_ip_intf_set(a_uint32_t dev_id,
		a_uint32_t index,
		fal_intf_entry_t *entry);

sw_error_t
fal_ip_intf_get(a_uint32_t dev_id,
    		a_uint32_t index,
    		fal_intf_entry_t *entry);

sw_error_t
fal_ip_vsi_intf_set(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id);

sw_error_t
fal_ip_vsi_intf_get(a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id);

sw_error_t
fal_ip_port_intf_set(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id);

sw_error_t
fal_ip_port_intf_get(a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id);

sw_error_t
fal_ip_nexthop_set(a_uint32_t dev_id, a_uint32_t index,
			fal_ip_nexthop_t *entry);

sw_error_t
fal_ip_nexthop_get(a_uint32_t dev_id, a_uint32_t index,
			fal_ip_nexthop_t *entry);

sw_error_t
fal_ip_vsi_sg_cfg_get(a_uint32_t dev_id, a_uint32_t vsi,
			fal_sg_cfg_t *sg_cfg);

sw_error_t
fal_ip_vsi_sg_cfg_set(a_uint32_t dev_id, a_uint32_t vsi,
			fal_sg_cfg_t *sg_cfg);

sw_error_t
fal_ip_port_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
			fal_sg_cfg_t *sg_cfg);

sw_error_t
fal_ip_port_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
			fal_sg_cfg_t *sg_cfg);

sw_error_t
fal_ip_pub_addr_set(a_uint32_t dev_id, a_uint32_t index,
			fal_ip_pub_addr_t *entry);

sw_error_t
fal_ip_pub_addr_get(a_uint32_t dev_id, a_uint32_t index,
			fal_ip_pub_addr_t *entry);

sw_error_t
fal_ip_port_macaddr_set(a_uint32_t dev_id, fal_port_t port_id,
			fal_macaddr_entry_t *macaddr);

sw_error_t
fal_ip_port_macaddr_get(a_uint32_t dev_id, fal_port_t port_id,
			fal_macaddr_entry_t *macaddr);

sw_error_t
fal_ip_route_mismatch_action_set(a_uint32_t dev_id, fal_fwd_cmd_t action);

sw_error_t
fal_ip_route_mismatch_action_get(a_uint32_t dev_id, fal_fwd_cmd_t *action);

sw_error_t
fal_ip_port_arp_sg_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
			fal_arp_sg_cfg_t *arp_sg_cfg);

sw_error_t
fal_ip_port_arp_sg_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
			fal_arp_sg_cfg_t *arp_sg_cfg);

sw_error_t
fal_ip_vsi_mc_mode_set(a_uint32_t dev_id, a_uint32_t vsi,
			fal_mc_mode_cfg_t *cfg);

sw_error_t
fal_ip_vsi_mc_mode_get(a_uint32_t dev_id, a_uint32_t vsi,
			fal_mc_mode_cfg_t *cfg);

sw_error_t
fal_ip_global_ctrl_get(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg);

sw_error_t
fal_ip_global_ctrl_set(a_uint32_t dev_id, fal_ip_global_cfg_t *cfg);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_IP_H_ */
/**
 * @}
 */
