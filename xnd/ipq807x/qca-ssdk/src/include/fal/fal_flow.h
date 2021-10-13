/*
 * Copyright (c) 2016-2019, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_flow
 * @{
 */
#ifndef _FAL_FLOW_H_
#define _FAL_FLOW_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "sw.h"
#include "fal/fal_type.h"
#include "fal/fal_ip.h"

typedef enum {
	FAL_FLOW_L3_UNICAST = 0,
	FAL_FLOW_L2_UNICAST,
	FAL_FLOW_MCAST,
} fal_flow_pkt_type_t;

typedef enum {
	FAL_FLOW_LAN_TO_LAN_DIR = 0,
	FAL_FLOW_LAN_TO_WAN_DIR,
	FAL_FLOW_WAN_TO_LAN_DIR,
	FAL_FLOW_WAN_TO_WAN_DIR,
	FAL_FLOW_UNKOWN_DIR_DIR,
} fal_flow_direction_t;

typedef enum {
	FAL_FLOW_FORWARD = 0,
	FAL_FLOW_SNAT,
	FAL_FLOW_DNAT,
	FAL_FLOW_ROUTE,
	FAL_FLOW_BRIDGE,
} fal_flow_fwd_type_t;

/* FLOW entry type field */
#define FAL_FLOW_IP4_5TUPLE_ADDR        0x1
#define FAL_FLOW_IP6_5TUPLE_ADDR        0x2
#define FAL_FLOW_IP4_3TUPLE_ADDR        0x4
#define FAL_FLOW_IP6_3TUPLE_ADDR        0x8

#define FAL_FLOW_OP_MODE_KEY         0x0
#define FAL_FLOW_OP_MODE_INDEX     0x1
#define FAL_FLOW_OP_MODE_FLUSH     0x2

#define FAL_FLOW_PROTOCOL_OTHER    0
#define FAL_FLOW_PROTOCOL_TCP	        1
#define FAL_FLOW_PROTOCOL_UDP	  2
#define FAL_FLOW_PROTOCOL_UDPLITE  3


typedef struct {
	fal_fwd_cmd_t miss_action; /* flow mismatch action*/
	a_bool_t frag_bypass_en; /*0 for disable and 1 for enable*/
	a_bool_t tcp_spec_bypass_en; /*0 for disable and 1 for enable*/
	a_bool_t all_bypass_en; /*0 for disable and 1 for enable*/
	a_uint8_t key_sel; /*0 for source ip address and 1 for destination ip address*/
} fal_flow_mgmt_t;

typedef struct {
	a_uint32_t entry_id; /*entry index*/ 
	a_uint8_t entry_type; /*1:ipv4 5 tuple, 2:ipv6 5 tuple, 4:ipv4 3 tuple, 8:ipv6 3 tuple*/
	a_uint8_t host_addr_type; /*0:souce ip index, 1:destination ip index*/
	a_uint16_t host_addr_index; /*host table entry index*/
	a_uint8_t protocol; /*1:tcp, 2:udp, 3:udp-lite, 0:other*/
	a_uint8_t age; /*aging value*/
	a_bool_t src_intf_valid; /*source interface check valid*/
	a_uint8_t src_intf_index; /*souce l3 interface*/
	a_uint8_t fwd_type; /*forward type*/
	a_uint16_t snat_nexthop; /*nexthop index for snat*/
	a_uint16_t snat_srcport; /*new source l4 port*/
	a_uint16_t dnat_nexthop; /*nexthop index for dnat*/
	a_uint16_t dnat_dstport; /*new destination l4 port*/
	a_uint16_t route_nexthop; /*nexthop index for route*/
	a_bool_t port_valid; /*route port valid*/
	fal_port_t route_port; /*port for route*/
	fal_port_t bridge_port; /*port for l2 bridge*/
	a_bool_t deacclr_en; /*0 for disable and 1 for enable*/
	a_bool_t copy_tocpu_en; /*0 for disable and 1 for enable*/
	a_uint8_t syn_toggle; /*update by host*/
	a_uint8_t pri_profile; /*flow qos index*/
	a_uint8_t sevice_code; /*service code for bypass*/
	a_uint8_t ip_type; /*0 for ipv4 and 1 for ipv6*/
	union {
		fal_ip4_addr_t ipv4;
		fal_ip6_addr_t ipv6;
	} flow_ip;
	a_uint16_t src_port; /*l4 source port*/
	a_uint16_t dst_port; /*l4 destination port*/
	a_uint32_t tree_id; /*for qos*/
	a_uint32_t pkt_counter; /*flow packet counter*/
	a_uint64_t byte_counter; /*flow byte counter*/
} fal_flow_entry_t;

typedef struct {
	fal_fwd_cmd_t src_if_check_action; /*source inferface check fail action*/
	a_bool_t src_if_check_deacclr_en; /*0 for disable and 1 for enable*/
	a_bool_t service_loop_en; /*0 for disable and 1 for enable*/
	fal_fwd_cmd_t service_loop_action; /*0 for disable and 1 for enable*/
	a_bool_t service_loop_deacclr_en; /*0 for disable and 1 for enable*/ 
	fal_fwd_cmd_t flow_deacclr_action; /*flow de acceleration action*/
	fal_fwd_cmd_t sync_mismatch_action; /*sync toggle mismatch action*/
	a_bool_t sync_mismatch_deacclr_en; /*0 for disable and 1 for enable*/
	a_uint8_t hash_mode_0; /*0 crc10, 1 xor, 2 crc16*/
	a_uint8_t hash_mode_1; /*0 crc10, 1 xor, 2 crc16*/
	a_bool_t flow_mismatch_copy_escape_en; /*0 for disable and 1 for enable*/
} fal_flow_global_cfg_t;

typedef struct {
	fal_flow_entry_t flow_entry;
	fal_host_entry_t host_entry;
} fal_flow_host_entry_t;

typedef struct {
	a_uint16_t age_time; /* age value*/
	a_uint16_t unit; /*0:second 1:cycle 2:million cycle*/
} fal_flow_age_timer_t;

enum  {
	FUNC_FLOW_HOST_ADD = 0,
	FUNC_FLOW_ENTRY_GET,
	FUNC_FLOW_ENTRY_DEL,
	FUNC_FLOW_STATUS_GET,
	FUNC_FLOW_CTRL_SET,
	FUNC_FLOW_AGE_TIMER_GET,
	FUNC_FLOW_STATUS_SET,
	FUNC_FLOW_HOST_GET,
	FUNC_FLOW_HOST_DEL,
	FUNC_FLOW_CTRL_GET,
	FUNC_FLOW_AGE_TIMER_SET,
	FUNC_FLOW_ENTRY_ADD,
	FUNC_FLOW_GLOBAL_CFG_GET,
	FUNC_FLOW_GLOBAL_CFG_SET,
	FUNC_FLOW_ENTRY_NEXT
};

#ifndef IN_FLOW_MINI
sw_error_t
fal_flow_status_set(a_uint32_t dev_id, a_bool_t enable);

sw_error_t
fal_flow_status_get(a_uint32_t dev_id, a_bool_t *enable);

sw_error_t
fal_flow_age_timer_set(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer);

sw_error_t
fal_flow_age_timer_get(a_uint32_t dev_id, fal_flow_age_timer_t *age_timer);
#endif

sw_error_t
fal_flow_mgmt_set(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *mgmt);

sw_error_t
fal_flow_mgmt_get(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *mgmt);

#ifndef IN_FLOW_MINI
sw_error_t
fal_flow_entry_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode, /*index or hash*/
		fal_flow_entry_t *flow_entry);

sw_error_t
fal_flow_entry_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_entry_t *flow_entry);

sw_error_t
fal_flow_entry_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_entry_t *flow_entry);

sw_error_t
fal_flow_entry_next(
		a_uint32_t dev_id,
		a_uint32_t next_mode,
		fal_flow_entry_t *flow_entry);

sw_error_t
fal_flow_host_add(
		a_uint32_t dev_id,
		a_uint32_t add_mode,
		fal_flow_host_entry_t *flow_host_entry);

sw_error_t
fal_flow_host_del(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_host_entry_t *flow_host_entry);

sw_error_t
fal_flow_host_get(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_host_entry_t *flow_host_entry);

sw_error_t
fal_flow_global_cfg_get(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg);

sw_error_t
fal_flow_global_cfg_set(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg);
#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_FLOW_H_ */

/**
 * @}
 */

