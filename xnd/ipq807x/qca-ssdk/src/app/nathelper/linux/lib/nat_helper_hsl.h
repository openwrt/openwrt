/*
 * Copyright (c) 2012, 2015, 2018, The Linux Foundation. All rights reserved.
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


#ifndef _NAT_HELPER_HSL_H
#define _NAT_HELPER_HSL_H

#ifdef KVER32
#include <linux/kconfig.h> 
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif

#include <linux/if_ether.h>
#include "fal_nat.h"

#define NAT_HW_NUM 32
#define NAT_HW_PORT_RANGE_MAX 255

#define FAL_NAT_ENTRY_PROTOCOL_TCP         0x1
#define FAL_NAT_ENTRY_PROTOCOL_UDP         0x2
#define FAL_NAT_ENTRY_PROTOCOL_PPTP        0x4
#define FAL_NAT_ENTRY_PROTOCOL_ANY         0x8
#define FAL_NAT_ENTRY_PORT_CHECK           0x20

#define MAX_INTF_NUM    4

/* WAN connection types */
#define NF_S17_WAN_TYPE_IP  0   /* DHCP, static IP connection */
#define NF_S17_WAN_TYPE_PPPOE   1   /* PPPoE connection */
#define NF_S17_WAN_TYPE_GRE 2   /* GRE connections, ex: PPTP */
#define NF_S17_WAN_TYPE_PPPOEV6 3   /* IPv6 PPPoE connection  using the same session as IPv4 connection */
#define NF_S17_WAN_TYPE_PPPOES0 4   /* PPPoE connection but not yet connected */
/* define the H/W Age mode for NAPT entries */
#define ARP_AGE_NEVER   7
#define ARP_AGE 6

#if !defined (HSL_STANDALONG)
/*NAT API*/
#define NAPT_ADD fal_napt_add
#define NAPT_GET fal_napt_get
#define NAT_PUB_ADDR_ADD fal_nat_pub_addr_add
#define NAPT_NEXT fal_napt_next
#define NAT_PRV_BASE_ADDR_SET fal_nat_prv_base_addr_set
#define NAT_PRV_BASE_MASK_SET fal_nat_prv_base_mask_set
#define NAPT_DEL fal_napt_del
#define NAT_DEL fal_nat_del
#define NAT_PUB_ADDR_DEL fal_nat_pub_addr_del
#define NAT_ADD fal_nat_add
#define NAT_PRV_ADDR_MODE_GET fal_nat_prv_addr_mode_get

/*IP API*/
#define IP_INTF_ENTRY_ADD fal_ip_intf_entry_add
#define IP_HOST_ADD fal_ip_host_add
#define IP_HOST_DEL fal_ip_host_del
#define IP_HOST_GET fal_ip_host_get
#define IP_HOST_NEXT fal_ip_host_next
#define IP_INTF_ENTRY_DEL fal_ip_intf_entry_del
#define IP_HOST_PPPOE_BIND fal_ip_host_pppoe_bind
#define IP_ROUTE_STATUS_SET fal_ip_route_status_set
#define IP_HOST_ROUTE_ADD fal_ip_host_route_set
#define IP_PRV_BASE_ADDR_SET fal_ip_vrf_base_addr_set
#define IP_PRV_BASE_MASK_SET fal_ip_vrf_base_mask_set

/* PPPOE */
#define PPPOE_STATUS_GET fal_pppoe_status_get
#define PPPOE_STATUS_SET fal_pppoe_status_set
#define PPPOE_SESSION_ID_SET fal_pppoe_session_id_set
#define PPPOE_SESSION_TABLE_ADD fal_pppoe_session_table_add
#define PPPOE_SESSION_TABLE_DEL fal_pppoe_session_table_del
#define RTD_PPPOE_EN_SET fal_rtd_pppoe_en_set

/*MISC API*/
#define MISC_ARP_CMD_SET fal_arp_cmd_set
#define CPU_VID_EN_SET fal_cpu_vid_en_set
#define PORT_ARP_ACK_STATUS_SET fal_port_arp_ack_status_set
#define CPU_PORT_STATUS_SET fal_cpu_port_status_set

/*ACL API*/
#define ACL_RULE_ADD fal_acl_rule_add
#define ACL_RULE_DEL fal_acl_rule_delete
#define ACL_LIST_CREATE fal_acl_list_creat
#define ACL_LIST_DESTROY fal_acl_list_destroy
#define ACL_LIST_BIND fal_acl_list_bind
#define ACL_LIST_UNBIND fal_acl_list_unbind
#define ACL_STATUS_GET fal_acl_status_get
#define ACL_STATUS_SET fal_acl_status_set
#define ACL_PORT_UDF_PROFILE_SET fal_acl_port_udf_profile_set

/*VLAN API */
#define VLAN_NEXT fal_vlan_next

/* PORTVLAN API */
#define PORTVLAN_ROUTE_DEFV_SET(dev_id, port_id)
#define NETISOLATE_SET fal_netisolate_set

/* PORT_CTRL API */
#define HEADER_TYPE_SET fal_header_type_set
#define PORT_TXHDR_MODE_SET fal_port_txhdr_mode_set

/* REG ACCESS API */
#define REG_GET fal_reg_get
#endif

extern int nf_athrs17_hnat;
extern int nf_athrs17_hnat_wan_type;
extern int nf_athrs17_hnat_ppp_id;
extern int nf_athrs17_hnat_udp_thresh;
extern a_uint32_t nf_athrs17_hnat_wan_ip;
extern a_uint32_t nf_athrs17_hnat_ppp_peer_ip;
extern unsigned char nf_athrs17_hnat_ppp_peer_mac[ETH_ALEN];
extern unsigned char nf_athrs17_hnat_wan_mac[ETH_ALEN];
extern int nf_athrs17_hnat_sync_counter_en;

extern int nf_athrs17_hnat_ppp_id2;
extern unsigned char nf_athrs17_hnat_ppp_peer_mac2[ETH_ALEN];

enum {
	NAT_CHIP_VER_8327 = 0x12,
	NAT_CHIP_VER_8337 = 0x13,
	NAT_CHIP_VER_DESS = 0x14,
};

typedef struct
{
    a_uint32_t entry_id;
    a_uint32_t flags;
    a_uint32_t src_addr;
    a_uint32_t trans_addr;
    a_uint16_t port_num;
    a_uint16_t port_range;
} nat_entry_t;

typedef struct
{
    a_uint32_t entry_id;
    a_uint32_t flags;
    a_uint32_t status;
    a_uint32_t src_addr;
    a_uint32_t dst_addr;
    a_uint16_t src_port;
    a_uint16_t dst_port;
    a_uint32_t trans_addr;
    a_uint16_t trans_port;
    a_uint32_t ingress_packet;
    a_uint32_t ingress_byte;
    a_uint32_t egress_packet;
    a_uint32_t egress_byte;
} napt_entry_t;

#if defined (__BIG_ENDIAN)
typedef struct
{
    a_uint16_t ver:2;
    a_uint16_t pri:3;
    a_uint16_t type:5;
    a_uint16_t rev:2;
    a_uint16_t with_tag:1;
    a_uint16_t sport:3;
    a_uint16_t vid;
    a_uint16_t magic;
} aos_header_t;
#elif defined (__LITTLE_ENDIAN)
typedef struct
{
    a_uint16_t vid;
    a_uint16_t sport:3;
    a_uint16_t with_tag:1;
    a_uint16_t rev:2;
    a_uint16_t type:5;
    a_uint16_t pri:3;
    a_uint16_t ver:2;
} aos_header_t;
#else
#error "no ENDIAN"
#endif

a_int32_t
nat_hw_add(nat_entry_t *nat);
a_int32_t
nat_hw_del_by_index(a_uint32_t index);
a_int32_t
nat_hw_flush(void);
a_int32_t
napt_hw_flush(void);
a_int32_t
nat_hw_prv_base_can_update(void);
void
nat_hw_prv_base_update_enable(void);
void
nat_hw_prv_base_update_disable(void);
a_int32_t
nat_hw_prv_base_set(a_uint32_t ip);
a_uint32_t
nat_hw_prv_base_get(void);
a_int32_t
nat_hw_prv_mask_set(a_uint32_t ipmask);
a_uint32_t
nat_hw_prv_mask_get(void);
a_int32_t
nat_hw_prv_base_is_match(a_uint32_t ip);
a_int32_t
if_mac_add(uint8_t *mac, uint32_t vid, uint32_t ipv6);
a_int32_t
if_mac_cleanup(void);
a_int32_t
arp_hw_add(a_uint32_t port, a_uint32_t intf_id, a_uint8_t *ip, a_uint8_t *mac, int is_ipv6_entry);
a_int32_t
arp_if_info_get(void *data, a_uint32_t *sport, a_uint32_t *vid);
a_int32_t
nat_hw_pub_ip_add(a_uint32_t ip, a_uint32_t *index);
void
napt_hw_mode_init(void);
void
napt_hw_mode_cleanup(void);
a_int32_t
nat_hw_pub_ip_del(a_uint32_t index);
a_int32_t
napt_hw_add(napt_entry_t *napt_entry);
a_int32_t
napt_hw_get(napt_entry_t *napt, fal_napt_entry_t *entry);
a_int32_t
napt_hw_dnat_cookie_add(napt_entry_t *napt, a_uint32_t cookie);
a_int32_t
napt_hw_snat_cookie_add(napt_entry_t *napt, a_uint32_t cookie);
a_int32_t
napt_hw_del(napt_entry_t *napt_entry);
a_int32_t
napt_hw_first_by_age(napt_entry_t *napt, a_uint32_t age);
a_int32_t
napt_hw_next_by_age(napt_entry_t *napt, a_uint32_t age);
a_int32_t
napt_hw_get_by_index(napt_entry_t *napt, a_uint16_t hw_index);
a_int32_t napt_hw_get_by_sip(a_uint32_t sip);
a_uint32_t
napt_hw_used_count_get(void);

sw_error_t napt_l3_status_set(a_uint32_t dev_id, a_bool_t enable);
sw_error_t napt_l3_status_get(a_uint32_t dev_id, a_bool_t * enable);

sw_error_t napt_helper_hsl_init(void);


#endif /*_NAT_HELPER_HSL_H*/

