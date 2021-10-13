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
 * @defgroup fal_ctrlpkt FAL_CTRLPKT
 * @{
 */
#ifndef _FAL_CTRLPKT_H_
#define _FAL_CTRLPKT_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

typedef struct {
	fal_fwd_cmd_t action; /* the action when condition matched */
	a_bool_t sg_bypass; /* check if sg_bypass when condition matched */
	a_bool_t l2_filter_bypass; /* check if l2_filter_bypass when condition matched */
	a_bool_t in_stp_bypass; /* check if in_stp_bypass when condition matched */
	a_bool_t in_vlan_fltr_bypass; /* check if in_vlan_fltr_bypass when condition matched */
} fal_ctrlpkt_action_t;

typedef struct
{
	a_bool_t mgt_eapol; /* eapol protocol management type */
	a_bool_t mgt_pppoe; /* pppoe protocol management type */
	a_bool_t mgt_igmp; /* igmp protocol management type */
	a_bool_t mgt_arp_req; /* arp request protocol management type */
	a_bool_t mgt_arp_rep; /* arp response protocol management type */
	a_bool_t mgt_dhcp4; /* dhcp4 protocol management type */
	a_bool_t mgt_mld; /* mld protocol management type */
	a_bool_t mgt_ns; /* ns protocol management type */
	a_bool_t mgt_na; /* na protocol management type */
	a_bool_t mgt_dhcp6; /* dhcp6 protocol management type */
} fal_ctrlpkt_protocol_type_t;

typedef struct {
	fal_ctrlpkt_action_t action; /* the all action when condition matched */
	fal_pbmp_t port_map; /* the condition port bitmap */
	a_uint32_t ethtype_profile_bitmap; /* the condition ethtype_profile bitmap */
	a_uint32_t rfdb_profile_bitmap; /* the condition rfdb_profile bitmap */
	fal_ctrlpkt_protocol_type_t protocol_types; /* the condition protocol types */
} fal_ctrlpkt_profile_t;

enum {
	FUNC_MGMTCTRL_ETHTYPE_PROFILE_SET = 0,
	FUNC_MGMTCTRL_ETHTYPE_PROFILE_GET,
	FUNC_MGMTCTRL_RFDB_PROFILE_SET,
	FUNC_MGMTCTRL_RFDB_PROFILE_GET,
	FUNC_MGMTCTRL_CTRLPKT_PROFILE_ADD,
	FUNC_MGMTCTRL_CTRLPKT_PROFILE_DEL,
	FUNC_MGMTCTRL_CTRLPKT_PROFILE_GETFIRST,
	FUNC_MGMTCTRL_CTRLPKT_PROFILE_GETNEXT,
};

sw_error_t fal_mgmtctrl_ethtype_profile_set(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t ethtype);
sw_error_t fal_mgmtctrl_ethtype_profile_get(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t * ethtype);

sw_error_t fal_mgmtctrl_rfdb_profile_set(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr);
sw_error_t fal_mgmtctrl_rfdb_profile_get(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr);

sw_error_t fal_mgmtctrl_ctrlpkt_profile_add(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt);
sw_error_t fal_mgmtctrl_ctrlpkt_profile_del(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt);
sw_error_t fal_mgmtctrl_ctrlpkt_profile_getfirst(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt);
sw_error_t fal_mgmtctrl_ctrlpkt_profile_getnext(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_CTRLPKT_H_ */
/**
 * @}
 */

