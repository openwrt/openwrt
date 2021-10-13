/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.

 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/*
 * nss_if_log.c
 *	NSS Interface logger file.
 */

#include "nss_core.h"

/*
 * nss_if_log_message_types_str
 *	NSS interface rule message strings
 */
static int8_t *nss_if_log_message_types_str[NSS_IF_MAX_MSG_TYPES] __maybe_unused = {
	"NSS interface Open message",
	"NSS interface close message",
	"NSS interface link state notify message",
	"NSS interface MTU change message",
	"NSS interface MAC address set message",
	"NSS interface stats message",
	"NSS interface ishaper assign message",
	"NSS interface bshaper assign message",
	"NSS interface ishaper unassign message",
	"NSS interface bshaper unassign message",
	"NSS interface ishaper config message",
	"NSS interface bshaper config message",
	"NSS interface pause on off message",
	"NSS interface VSI assign message",
	"NSS interface VSI unassign message",
	"NSS interface set next hop message",
	"NSS interface set IGS node message",
	"NSS interface clear IGS node message",
	"NSS interface reset next hop message",
};

/*
 * nss_if_log_error_response_types_str
 *	Strings for error types for NSS interface messages
 */
static int8_t *nss_if_log_error_response_types_str[NSS_IF_ERROR_TYPE_MAX] __maybe_unused = {
	"No Ishapers",
	"No Bshapers",
	"No Ishaper",
	"No Bshaper",
	"No Old Ishaper",
	"No Old Bshaper",
	"Ishaper config failed",
	"Bshaper config failed",
	"Unknown error",
	"Interface open error",
	"Interface invalid MTU error",
	"Invalid MAC address error",
	"VSI no match error",
	"VSI reassign error",
	"Invalid VSI error",
	"Max error",
};

/*
 * nss_if_log_rule_open()
 *	Log NSS open interface message.
 */
static void nss_if_log_rule_open(struct nss_if_msg *nim)
{
	struct nss_if_open *niom __maybe_unused = &nim->msg.open;
	nss_trace("%px: NSS open interface message \n"
		"tx_desc_ring: %X\n"
		"rx_desc_ring: %X\n"
		"rx_forward_if: %u\n"
		"alignment_mode: %u\n",
		nim,
		niom->tx_desc_ring,
		niom->rx_desc_ring,
		niom->rx_forward_if,
		niom->alignment_mode);
}

/*
 * nss_if_log_rule_close()
 *	Log NSS close interface message.
 */
static void nss_if_log_rule_close(struct nss_if_msg *nim)
{
	nss_trace("%px: NSS close interface message \n", nim);
}

/*
 * nss_if_log_rule_link_state_notify()
 *	Log NSS interface link state notify message.
 */
static void nss_if_log_rule_link_state_notify(struct nss_if_msg *nim)
{
	struct nss_if_link_state_notify *nilstm __maybe_unused = &nim->msg.link_state_notify;
	nss_trace("%px: NSS interface link state notify interface message \n"
		"state: %u\n",
		nim,
		nilstm->state);
}

/*
 * nss_if_log_rule_mtu_change()
 *	Log NSS interface MTU change message.
 */
static void nss_if_log_rule_mtu_change(struct nss_if_msg *nim)
{
	struct nss_if_mtu_change *nimcm __maybe_unused = &nim->msg.mtu_change;
	nss_trace("%px: NSS interface MTU change message \n"
		"min_buf_size: %u\n",
		nim,
		nimcm->min_buf_size);
}

/*
 * nss_if_log_rule_mac_addr_set()
 *	Log NSS interface MAC address set message.
 */
static void nss_if_log_rule_mac_addr_set(struct nss_if_msg *nim)
{
	struct nss_if_mac_address_set *nimasm __maybe_unused = &nim->msg.mac_address_set;
	nss_trace("%px: NSS interface MAC address set message \n"
		"MAC address: %X:%X:%X:%X:%X:%X\n",
		nim,
		nimasm->mac_addr[0], nimasm->mac_addr[1], nimasm->mac_addr[2],
		nimasm->mac_addr[3], nimasm->mac_addr[4], nimasm->mac_addr[5]);
}

/*
 * nss_if_log_rule_stats()
 *	Log NSS interface stats message.
 */
static void nss_if_log_rule_stats(struct nss_if_msg *nim)
{
	uint16_t i;
	struct nss_cmn_node_stats *nism __maybe_unused = &nim->msg.stats;

	nss_trace("%px: NSS interface stats message \n"
		"rx_packets: %u\n"
		"rx_bytes: %u\n"
		"tx_packets: %u\n"
		"tx_bytes: %u\n",
		nim,
		nism->rx_packets,
		nism->rx_bytes,
		nism->tx_packets,
		nism->tx_bytes);

	for(i=0; i < NSS_MAX_NUM_PRI; i++)
	{
		nss_trace("rx_dropped[%u]: %u\n", i, nism->rx_dropped[i]);
	}
}

/*
 * nss_if_log_rule_shaper_assign()
 *	Log NSS interface shaper assignment message.
 */
static void nss_if_log_rule_shaper_assign(struct nss_if_msg *nim)
{
	struct nss_if_shaper_assign *shaper_assign_msg __maybe_unused = &nim->msg.shaper_assign;
	nss_trace("%px: NSS interface shaper assign message \n"
		"shaper_id: %u\n"
		"new_shaper_id: %u\n",
		nim,
		shaper_assign_msg->shaper_id,
		shaper_assign_msg->new_shaper_id);
}

/*
 * nss_if_log_rule_shaper_unassign()
 *	Log NSS interface shaper unassignment message.
 */
static void nss_if_log_rule_shaper_unassign(struct nss_if_msg *nim)
{
	struct nss_if_shaper_unassign *shaper_unassign_msg __maybe_unused = &nim->msg.shaper_unassign;
	nss_trace("%px: NSS interface shaper unassign message \n"
		"shaper_id: %u\n",
		nim,
		shaper_unassign_msg->shaper_id);
}

/*
 * nss_if_log_rule_shaper_config()
 *	Log NSS interface shaper configuration message.
 */
static void nss_if_log_rule_shaper_config(struct nss_if_msg *nim)
{
	struct nss_if_shaper_configure *shaper_configure_msg __maybe_unused = &nim->msg.shaper_configure;
	nss_trace("%px: NSS interface shaper configuration message \n"
		"request_type: %u\n"
		"response_type: %u\n",
		nim,
		shaper_configure_msg->config.request_type,
		shaper_configure_msg->config.response_type);
}

/*
 * nss_if_log_rule_pause_on_off()
 *	Log NSS interface pause on off message.
 */
static void nss_if_log_rule_pause_on_off(struct nss_if_msg *nim)
{
	struct nss_if_pause_on_off *pause_on_off_msg __maybe_unused = &nim->msg.pause_on_off;
	nss_trace("%px: NSS interface pause ON/OFF message \n"
		"pause_on: %u\n",
		nim,
		pause_on_off_msg->pause_on);
}

/*
 * nss_if_log_rule_vsi_assign()
 *	Log NSS interface VSI assignment message.
 */
static void nss_if_log_rule_vsi_assign(struct nss_if_msg *nim)
{
	struct nss_if_vsi_assign *vsi_assign_msg __maybe_unused = &nim->msg.vsi_assign;
	nss_trace("%px: NSS interface VSI assignment message \n"
		"VSI: %u\n",
		nim,
		vsi_assign_msg->vsi);
}

/*
 * nss_if_log_rule_vsi_unassign()
 *	Log NSS interface VSI unassignment message.
 */
static void nss_if_log_rule_vsi_unassign(struct nss_if_msg *nim)
{
	struct nss_if_vsi_unassign *vsi_unassign_msg __maybe_unused = &nim->msg.vsi_unassign;
	nss_trace("%px: NSS interface VSI unassignment message \n"
		"VSI: %u\n",
		nim,
		vsi_unassign_msg->vsi);
}

/*
 * nss_if_log_rule_set_nexthop()
 *	Log NSS interface set nexthop message.
 */
static void nss_if_log_rule_set_nexthop(struct nss_if_msg *nim)
{
	struct nss_if_set_nexthop *nisn __maybe_unused = &nim->msg.set_nexthop;
	nss_trace("%px: NSS interface set nethop message \n"
		"Nexthop: %u\n",
		nim,
		nisn->nexthop);
}

/*
 * nss_if_log_rule_set_igs_node()
 *	Log NSS interface set IGS node message.
 */
static void nss_if_log_rule_set_igs_node(struct nss_if_msg *nim)
{
	struct nss_if_igs_config *igs_config_msg __maybe_unused = &nim->msg.config_igs;
	nss_trace("%px: NSS interface set IGS node message \n"
		"igs_num: %d\n",
		nim,
		igs_config_msg->igs_num);
}

/*
 * nss_if_log_rule_clear_igs_node()
 *	Log NSS interface clear IGS node message.
 */
static void nss_if_log_rule_clear_igs_node(struct nss_if_msg *nim)
{
	struct nss_if_igs_config *igs_config_msg __maybe_unused = &nim->msg.config_igs;
	nss_trace("%px: NSS interface clear IGS node message \n"
		"igs_num: %d\n",
		nim,
		igs_config_msg->igs_num);
}

/*
 * nss_if_log_rule_reset_nexthop()
 *	Log NSS interface reset nexthop message.
 */
static void nss_if_log_rule_reset_nexthop(struct nss_if_msg *nim)
{
	nss_trace("%px: NSS interface reset nexthop message \n", nim);
}

/*
 * nss_if_log_verbose()
 *	Log message contents.
 */
static void nss_if_log_verbose(struct nss_if_msg *nim)
{
	nss_trace("NSS interface number: %u\n", nim->cm.interface);

	switch (nim->cm.type) {
	case NSS_IF_OPEN:
		nss_if_log_rule_open(nim);
		break;

	case NSS_IF_CLOSE:
		nss_if_log_rule_close(nim);
		break;

	case NSS_IF_LINK_STATE_NOTIFY:
		nss_if_log_rule_link_state_notify(nim);
		break;

	case NSS_IF_MTU_CHANGE:
		nss_if_log_rule_mtu_change(nim);
		break;

	case NSS_IF_MAC_ADDR_SET:
		nss_if_log_rule_mac_addr_set(nim);
		break;

	case NSS_IF_STATS:
		nss_if_log_rule_stats(nim);
		break;

	case NSS_IF_ISHAPER_ASSIGN:
	case NSS_IF_BSHAPER_ASSIGN:
		nss_if_log_rule_shaper_assign(nim);
		break;

	case NSS_IF_ISHAPER_UNASSIGN:
	case NSS_IF_BSHAPER_UNASSIGN:
		nss_if_log_rule_shaper_unassign(nim);
		break;

	case NSS_IF_ISHAPER_CONFIG:
	case NSS_IF_BSHAPER_CONFIG:
		nss_if_log_rule_shaper_config(nim);
		break;

	case NSS_IF_PAUSE_ON_OFF:
		nss_if_log_rule_pause_on_off(nim);
		break;

	case NSS_IF_VSI_ASSIGN:
		nss_if_log_rule_vsi_assign(nim);
		break;

	case NSS_IF_VSI_UNASSIGN:
		nss_if_log_rule_vsi_unassign(nim);
		break;

	case NSS_IF_SET_NEXTHOP:
		nss_if_log_rule_set_nexthop(nim);
		break;

	case NSS_IF_SET_IGS_NODE:
		nss_if_log_rule_set_igs_node(nim);
		break;

	case NSS_IF_CLEAR_IGS_NODE:
		nss_if_log_rule_clear_igs_node(nim);
		break;

	case NSS_IF_RESET_NEXTHOP:
		nss_if_log_rule_reset_nexthop(nim);
		break;

	default:
		nss_trace("%px: Invalid message type\n", nim);
		break;
	}
}

/*
 * nss_if_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_if_log_rx_msg(struct nss_if_msg *nim)
{
	if (nim->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_info("%px: Invalid response\n", nim);
		return;
	}

	if (nim->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nim->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nim, nim->cm.type,
			nss_if_log_message_types_str[nim->cm.type],
			nim->cm.response, nss_cmn_response_str[nim->cm.response]);
			goto verbose;
	}

	if (nim->cm.error >= NSS_IF_ERROR_TYPE_MAX) {
		nss_info("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
		nim, nim->cm.type, nss_if_log_message_types_str[nim->cm.type],
			nim->cm.response, nss_cmn_response_str[nim->cm.response],
			nim->cm.error);
		goto verbose;
	}

	nss_info("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nim, nim->cm.type, nss_if_log_message_types_str[nim->cm.type],
		nim->cm.response, nss_cmn_response_str[nim->cm.response],
		nim->cm.error, nss_if_log_error_response_types_str[nim->cm.error]);

verbose:
	nss_if_log_verbose(nim);
}

/*
 * nss_if_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_if_log_tx_msg(struct nss_if_msg *nim)
{
	if (nim->cm.type >= NSS_IF_MAX_MSG_TYPES) {
		nss_info("%px: Invalid message type\n", nim);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nim, nim->cm.type, nss_if_log_message_types_str[nim->cm.type]);
	nss_if_log_verbose(nim);
}
