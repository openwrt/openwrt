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
 * @defgroup fal_servcode FAL_SERVCODE
 * @{
 */
#ifndef _FAL_SERVCODE_H_
#define _FAL_SERVCODE_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

enum {
	FLD_UPDATE_CAPWAP_EN = 0, /*only for IP197*/
	FLD_UPDATE_DIRECTION, /*only for IP197*/
	FLD_UPDATE_DEST_INFO,
	FLD_UPDATE_SRC_INFO,
	FLD_UPDATE_INT_PRI,
	FLD_UPDATE_SERVICE_CODE,
	FLD_UPDATE_HASH_FLOW_INDEX,
	FLD_UPDATE_FAKE_L2_PROT_EN, /*only for IP197*/
};

enum {
	IN_VLAN_TAG_FMT_CHECK_BYP = 0,
	IN_VLAN_MEMBER_CHECK_BYP,
	IN_VLAN_XLT_BYP,
	MY_MAC_CHECK_BYP,
	DIP_LOOKUP_BYP,
	FLOW_LOOKUP_BYP = 5,
	FLOW_ACTION_BYP,
	ACL_BYP,
	FAKE_MAC_HEADER_BYP,
	SERVICE_CODE_BYP,
	WRONG_PKT_FMT_L2_BYP = 10,
	WRONG_PKT_FMT_L3_IPV4_BYP,
	WRONG_PKT_FMT_L3_IPV6_BYP,
	WRONG_PKT_FMT_L4_BYP,
	FLOW_SERVICE_CODE_BYP,
	ACL_SERVICE_CODE_BYP = 15,
	FAKE_L2_PROTO_BYP,
	PPPOE_TERMINATION_BYP,
	DEFAULT_VLAN_BYP,
	IN_VLAN_ASSIGN_FAIL_BYP = 24,
	SOURCE_GUARD_BYP,
	MRU_MTU_CHECK_BYP,
	FLOW_SRC_CHECK_BYP,
	FLOW_QOS_BYP,

	EG_VLAN_MEMBER_CHECK_BYP = 32,
	EG_VLAN_XLT_BYP,
	EG_VLAN_TAG_FMT_CTRL_BYP,
	FDB_LEARN_BYP = 35,
	FDB_REFRESH_BYP,
	L2_SOURCE_SEC_BYP,
	MANAGEMENT_FWD_BYP,
	BRIDGING_FWD_BYP,
	IN_STP_FLTR_BYP = 40,
	EG_STP_FLTR_BYP,
	SOURCE_FLTR_BYP,
	POLICER_BYP,
	L2_PKT_EDIT_BYP,
	L3_PKT_EDIT_BYP = 45,
	ACL_POST_ROUTING_CHECK_BYP,
	PORT_ISOLATION_BYP,

	RX_VLAN_COUNTER_BYP = 64,
	RX_COUNTER_BYP,
	TX_VLAN_COUNTER_BYP,
	TX_COUNTER_BYP,
};

typedef struct {
	a_bool_t dest_port_valid; /* dest_port_id valid or not */
	fal_port_t dest_port_id; /* destination physical port id:0-7 */
	a_uint32_t  bypass_bitmap[3]; /* refer to enum IN_VLAN_TAG_FMT_CHECK_BYP... */
	a_uint32_t  direction; /* if dest is vp, fill it in dest_info or src_info, 0:dest, 1:src */

	a_uint32_t  field_update_bitmap; /* refer to enum FLD_UPDATE_CAPWAP_EN... */
	a_uint32_t  next_service_code; /* next service code */
	a_uint32_t  hw_services; /* HW_SERVICES to IP-197 */
	a_uint32_t  offset_sel; /* Select the offset value to IP-197:0: l3_offset, 1:l4_offset */
} fal_servcode_config_t;

enum
{
	/*servcode*/
	FUNC_SERVCODE_CONFIG_SET = 0,
	FUNC_SERVCODE_CONFIG_GET,
	FUNC_SERVCODE_LOOPCHECK_EN,
	FUNC_SERVCODE_LOOPCHECK_STATUS_GET,
};

sw_error_t fal_servcode_config_set(a_uint32_t dev_id, a_uint32_t servcode_index, fal_servcode_config_t *entry);
sw_error_t fal_servcode_config_get(a_uint32_t dev_id, a_uint32_t servcode_index, fal_servcode_config_t *entry);
sw_error_t fal_servcode_loopcheck_en(a_uint32_t dev_id, a_bool_t enable);
sw_error_t fal_servcode_loopcheck_status_get(a_uint32_t dev_id, a_bool_t *enable);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_SERVCODE_H_ */
/**
 * @}
 */


