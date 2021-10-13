/*
 * Copyright (c) 2012, 2015, The Linux Foundation. All rights reserved.
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

#ifndef _HSL_SHARED_API_H
#define _HSL_SHARED_API_H

#include "hsl.h"
#include "hsl_api.h"
#include "ssdk_init.h"

extern ssdk_chip_type SSDK_CURRENT_CHIP_TYPE;  /*running chip type*/

#if !defined (HSL_STANDALONG)
/*MISC API*/
#define PORT_BC_FILTER_SET fal_port_bc_filter_set
#define PORT_UNK_MC_FILTER_SET fal_port_unk_mc_filter_set
#define PORT_UNK_UC_FILTER_SET fal_port_unk_uc_filter_set

/*ACL API*/
#define ACL_RULE_QUERY fal_acl_rule_query

/*VLAN API */
#define VLAN_CREATE fal_vlan_create
#define VLAN_DEL fal_vlan_delete
#define VLAN_FIND fal_vlan_find
#define VLAN_MEMBER_ADD fal_vlan_member_add

/*RATE API*/
#define RATE_ACL_POLICER_SET fal_rate_acl_policer_set

/*MIB API*/
#define MIB_STATUS_SET fal_mib_status_set
#define GET_MIB_INFO fal_get_mib_info

/* PORT_CTRL API */
#define PORT_TXMAC_STATUS_SET fal_port_txmac_status_set
#define PORT_RXMAC_STATUS_SET fal_port_rxmac_status_set

#elif defined(ISISC)
/* NAT API*/
#include "isisc_nat.h"
#define NAPT_ADD isisc_napt_add
#define NAT_PUB_ADDR_ADD isisc_nat_pub_addr_add
#define NAPT_NEXT isisc_napt_next
#define NAT_PRV_BASE_ADDR_SET isisc_nat_prv_base_addr_set
#define NAT_PRV_BASE_MASK_SET isisc_nat_prv_base_mask_set
#define NAPT_DEL isisc_napt_del
#define NAT_DEL isisc_nat_del
#define NAT_PUB_ADDR_DEL isisc_nat_pub_addr_del
#define NAT_ADD isisc_nat_add
#define NAT_PRV_ADDR_MODE_GET isisc_nat_prv_addr_mode_get

/*IP API*/
#include "isisc_ip.h"
#define IP_INTF_ENTRY_ADD isisc_ip_intf_entry_add
#define IP_HOST_ADD isisc_ip_host_add
#define IP_HOST_DEL isisc_ip_host_del
#define IP_HOST_GET isisc_ip_host_get
#define IP_HOST_NEXT isisc_ip_host_next
#define IP_INTF_ENTRY_DEL isisc_ip_intf_entry_del
#define IP_HOST_PPPOE_BIND isisc_ip_host_pppoe_bind
#define IP_ROUTE_STATUS_SET isisc_ip_route_status_set

/*MISC API*/
#include "isisc_misc.h"
#define PPPOE_STATUS_GET isisc_pppoe_status_get
#define PPPOE_STATUS_SET isisc_pppoe_status_set
#define PPPOE_SESSION_ID_SET isisc_pppoe_session_id_set
#define PPPOE_SESSION_TABLE_ADD isisc_pppoe_session_table_add
#define PPPOE_SESSION_TABLE_DEL isisc_pppoe_session_table_del
#define PORT_BC_FILTER_SET  isisc_port_bc_filter_set
#define PORT_UNK_MC_FILTER_SET  isisc_port_unk_mc_filter_set
#define PORT_UNK_UC_FILTER_SET isisc_port_unk_uc_filter_set
#define PORT_RXMAC_STATUS_SET isisc_port_rxmac_status_set
#define MISC_ARP_CMD_SET isisc_arp_cmd_set
#define MISC_ARP_SP_NOT_FOUND_SET isisc_arp_unk_source_cmd_set
#define MISC_ARP_GUARD_SET isisc_ip_arp_guard_set
#define CPU_VID_EN_SET isisc_cpu_vid_en_set
#define RTD_PPPOE_EN_SET isisc_rtd_pppoe_en_set
#define PORT_ARP_ACK_STATUS_SET isisc_port_arp_ack_status_set
#define CPU_PORT_STATUS_SET isisc_cpu_port_status_set

/*ACL API*/
#include "isisc_acl.h"
#define ACL_RULE_ADD isisc_acl_rule_add
#define ACL_RULE_DEL isisc_acl_rule_delete
#define ACL_LIST_CREATE  isisc_acl_list_creat
#define ACL_LIST_DESTROY isisc_acl_list_destroy
#define ACL_LIST_BIND isisc_acl_list_bind
#define ACL_LIST_UNBIND  isisc_acl_list_unbind
#define ACL_RULE_GET_OFFSET isisc_acl_rule_get_offset
#define ACL_RULE_QUERY isisc_acl_rule_query
#define ACL_RULE_SYNC_MULTI_PORTMAP isisc_acl_rule_sync_multi_portmap
#define ACL_STATUS_GET isisc_acl_status_get
#define ACL_STATUS_SET isisc_acl_status_set
#define ACL_PORT_UDF_PROFILE_SET isisc_acl_port_udf_profile_set

/*VLAN API */
#include "isisc_vlan.h"
#define VLAN_CREATE isisc_vlan_create
#define VLAN_DEL isisc_vlan_delete
#define VLAN_FIND isisc_vlan_find
#define VLAN_MEMBER_ADD isisc_vlan_member_add

/*RATE API*/
#include "isisc_rate.h"
#define RATE_ACL_POLICER_SET isisc_rate_acl_policer_set

/*MIB API*/
#include "isisc_mib.h"
#define MIB_STATUS_SET isisc_mib_status_set
#define GET_MIB_INFO isisc_get_mib_info

/* PORTVLAN API */
#include "isisc_portvlan.h"
#define PORTVLAN_ROUTE_DEFV_SET isisc_port_route_defv_set
#define NETISOLATE_SET isisc_netisolate_set

/* PORT_CTRL API */
#include "isisc_port_ctrl.h"
#define HEADER_TYPE_SET isisc_header_type_set
#define PORT_TXHDR_MODE_SET isisc_port_txhdr_mode_set
#define PORT_TXMAC_STATUS_SET isisc_port_txmac_status_set

#elif defined(ISIS)
/* NAT API*/
#include "isis_nat.h"
#define NAPT_ADD isis_napt_add
#define NAT_PUB_ADDR_ADD isis_nat_pub_addr_add
#define NAPT_NEXT isis_napt_next
#define NAT_PRV_BASE_ADDR_SET isis_nat_prv_base_addr_set
#define NAT_PRV_BASE_MASK_SET isis_nat_prv_base_mask_set
#define NAPT_DEL isis_napt_del
#define NAT_DEL isis_nat_del
#define NAT_PUB_ADDR_DEL isis_nat_pub_addr_del
#define NAT_ADD isis_nat_add
#define NAT_PRV_ADDR_MODE_GET isis_nat_prv_addr_mode_get

/*IP API*/
#include "isis_ip.h"
#define IP_INTF_ENTRY_ADD isis_ip_intf_entry_add
#define IP_HOST_ADD isis_ip_host_add
#define IP_HOST_DEL isis_ip_host_del
#define IP_HOST_GET isis_ip_host_get
#define IP_HOST_NEXT isis_ip_host_next
#define IP_INTF_ENTRY_DEL isis_ip_intf_entry_del
#define IP_HOST_PPPOE_BIND isis_ip_host_pppoe_bind
#define IP_ROUTE_STATUS_SET isis_ip_route_status_set

/*MISC API*/
#include "isis_misc.h"
#define PPPOE_STATUS_GET isis_pppoe_status_get
#define PPPOE_STATUS_SET isis_pppoe_status_set
#define PPPOE_SESSION_ID_SET isis_pppoe_session_id_set
#define PPPOE_SESSION_TABLE_ADD isis_pppoe_session_table_add
#define PPPOE_SESSION_TABLE_DEL isis_pppoe_session_table_del
#define PORT_BC_FILTER_SET  isis_port_bc_filter_set
#define PORT_UNK_MC_FILTER_SET  isis_port_unk_mc_filter_set
#define PORT_UNK_UC_FILTER_SET isis_port_unk_uc_filter_set
#define PORT_RXMAC_STATUS_SET isis_port_rxmac_status_set
#define MISC_ARP_CMD_SET isis_arp_cmd_set
#define CPU_VID_EN_SET isis_cpu_vid_en_set
#define RTD_PPPOE_EN_SET isis_rtd_pppoe_en_set
#define PORT_ARP_ACK_STATUS_SET isis_port_arp_ack_status_set
#define CPU_PORT_STATUS_SET isis_cpu_port_status_set

/*ACL API*/
#include "isis_acl.h"
#define ACL_RULE_ADD isis_acl_rule_add
#define ACL_RULE_DEL isis_acl_rule_delete
#define ACL_LIST_CREATE  isis_acl_list_creat
#define ACL_LIST_DESTROY isis_acl_list_destroy
#define ACL_LIST_BIND isis_acl_list_bind
#define ACL_LIST_UNBIND  isis_acl_list_unbind
#define ACL_RULE_GET_OFFSET isis_acl_rule_get_offset
#define ACL_RULE_QUERY isis_acl_rule_query
#define ACL_RULE_SYNC_MULTI_PORTMAP isis_acl_rule_sync_multi_portmap
#define ACL_STATUS_GET isis_acl_status_get
#define ACL_STATUS_SET isis_acl_status_set
#define ACL_PORT_UDF_PROFILE_SET isis_acl_port_udf_profile_set

/*VLAN API */
#include "isis_vlan.h"
#define VLAN_CREATE isis_vlan_create
#define VLAN_DEL isis_vlan_delete
#define VLAN_FIND isis_vlan_find
#define VLAN_MEMBER_ADD isis_vlan_member_add

/*RATE API*/
#include "isis_rate.h"
#define RATE_ACL_POLICER_SET isis_rate_acl_policer_set

/*MIB API*/
#include "isis_mib.h"
#define MIB_STATUS_SET isis_mib_status_set
#define GET_MIB_INFO isis_get_mib_info

/* PORTVLAN API */
#include "isis_portvlan.h"
#define PORTVLAN_ROUTE_DEFV_SET isis_port_route_defv_set

/* PORT_CTRL API */
#include "isis_port_ctrl.h"
#define HEADER_TYPE_SET isis_header_type_set
#define PORT_TXHDR_MODE_SET isis_port_txhdr_mode_set
#define PORT_TXMAC_STATUS_SET isis_port_txmac_status_set
#endif

#endif
