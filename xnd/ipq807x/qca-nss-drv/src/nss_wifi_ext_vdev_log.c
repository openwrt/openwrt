/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
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
 * nss_wifi_ext_vdev_log.c
 *	NSS WiFi extended VAP logger file.
 */

#include "nss_core.h"

#define NSS_WIFI_EXT_VDEV_LOG_MESSAGE_TYPE_INDEX(type) ((type) - NSS_IF_MAX_MSG_TYPES)

/*
 * nss_wifi_ext_vdev_log_message_types_str
 *	NSS WiFi extended VAP message strings
 */
static int8_t *nss_wifi_ext_vdev_log_message_types_str[NSS_WIFI_EXT_VDEV_MSG_MAX] __maybe_unused = {
	"WiFi Common I/F Message",
	"WiFi Extendev VAP configure",
	"WiFi Extendev VAP configure wds",
	"WiFi Extendev VAP configure next hop",
	"WiFi Extendev VAP stats",
	"WiFi Extended VAP configure VLAN"
};

/*
 * nss_wifi_ext_vdev_log_configure_msg()
 *	Log NSS WiFi extended vap configure message.
 */
static void nss_wifi_ext_vdev_log_configure_if_msg(struct nss_wifi_ext_vdev_msg *nwevm)
{
	struct nss_wifi_ext_vdev_configure_if_msg *cmsg __maybe_unused = &nwevm->msg.cmsg;
	nss_trace("%px: WiFi extended VAP configure message \n"
		"Mac address: %pM\n"
		"Radio interface num: %d\n"
		"Parent VAP interface num: %d\n",
		cmsg, cmsg->mac_addr, cmsg->radio_ifnum,
		cmsg->pvap_ifnum);

}

/*
 * nss_wifi_ext_vdev_log_wds_msg()
 *	Log NSS WiFi extended vap wds message.
 */
static void nss_wifi_ext_vdev_log_wds_msg(struct nss_wifi_ext_vdev_msg *nwevm)
{
	struct nss_wifi_ext_vdev_wds_msg *wmsg __maybe_unused = &nwevm->msg.wmsg;
	nss_trace("%px: NSS WiFi extended VAP wds message: \n"
		"WDS sta ID: %d\n"
		"WDS sta macaddr: %pM\n",
		wmsg, wmsg->wds_peer_id,
		wmsg->mac_addr);
}

/*
 * nss_wifi_ext_vdev_set_nxt_hop_msg()
 *	Set the next hop message.
 */
static void nss_wifi_ext_vdev_set_nxt_hop_msg(struct nss_wifi_ext_vdev_msg *nwevm)
{
	struct nss_wifi_ext_vdev_set_next_hop_msg *wnhm __maybe_unused = &nwevm->msg.wnhm;
	nss_trace("%px: NSS WiFi extended vap set next hop message: \n"
		"Next hop if num: %d\n",
		wnhm, wnhm->if_num);

}

/*
 * nss_wifi_ext_vdev_linkup_msg()
 *	Log NSS linkup message.
 */
static void nss_wifi_ext_vdev_linkup_msg(struct nss_wifi_ext_vdev_msg *nwevm)
{
	union nss_if_msgs *if_msg __maybe_unused = &nwevm->msg.if_msg;
	nss_trace("%px: NSS WiFi ext linkup message\n", if_msg);
}

/*
 * nss_wifi_ext_vdev_linkdown_msg()
 *	Log NSS linkdown message.
 */
static void nss_wifi_ext_vdev_linkdown_msg(struct nss_wifi_ext_vdev_msg *nwevm)
{
	union nss_if_msgs *if_msg __maybe_unused = &nwevm->msg.if_msg;
	nss_trace("%px: NSS WiFi ext linkdown message\n", if_msg);
}

/*
 * nss_wifi_ext_vdev_macaddr_set_msg()
 *	Set/Change the mac address
 */
static void nss_wifi_ext_vdev_macaddr_set_msg(struct nss_wifi_ext_vdev_msg *nwevm)
{
	union nss_if_msgs *if_msg = &nwevm->msg.if_msg;
	struct nss_if_mac_address_set *nimas __maybe_unused = &if_msg->mac_address_set;
	nss_trace("%px: NSS WiFi ext change mac addr: \n"
		"mac addr %pM\n",
		nimas, nimas->mac_addr);
}

/*
 * nss_wifi_ext_vdev_log_vlan_msg()
 *	Configure vlan message.
 */
static void nss_wifi_ext_vdev_log_vlan_msg(struct nss_wifi_ext_vdev_msg *nwevm)
{
	struct nss_wifi_ext_vdev_vlan_msg *vmsg __maybe_unused = &nwevm->msg.vmsg;
	nss_trace("%px: NSS WiFi extended VAP vlan message: \n"
		"vlan ID %hu\n",
		vmsg, vmsg->vlan_id);
}

/*
 * nss_wifi_ext_vdev_log_verbose()
 *	Log message contents.
 */
static void nss_wifi_ext_vdev_log_verbose(struct nss_wifi_ext_vdev_msg *nwevm)
{
	switch (nwevm->cm.type) {
	case NSS_WIFI_EXT_VDEV_MSG_CONFIGURE_IF:
		nss_wifi_ext_vdev_log_configure_if_msg(nwevm);
		break;

	case NSS_WIFI_EXT_VDEV_MSG_CONFIGURE_WDS :
		nss_wifi_ext_vdev_log_wds_msg(nwevm);
		break;

	case NSS_WIFI_EXT_VDEV_SET_NEXT_HOP:
		nss_wifi_ext_vdev_set_nxt_hop_msg(nwevm);
		break;

	case NSS_WIFI_EXT_VDEV_MSG_STATS_SYNC:
		break;

	case NSS_IF_OPEN:
		nss_wifi_ext_vdev_linkup_msg(nwevm);
		break;

	case NSS_IF_CLOSE:
		nss_wifi_ext_vdev_linkdown_msg(nwevm);
		break;

	case NSS_IF_MAC_ADDR_SET:
		nss_wifi_ext_vdev_macaddr_set_msg(nwevm);
		break;

	case NSS_WIFI_EXT_VDEV_MSG_CONFIGURE_VLAN:
		nss_wifi_ext_vdev_log_vlan_msg(nwevm);
		break;

	default:
		nss_trace("%px: Invalid message type\n", nwevm);
		break;
	}
}

/*
 * nss_wifi_ext_vdev_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_wifi_ext_vdev_log_tx_msg(struct nss_wifi_ext_vdev_msg *nwevm)
{
	uint32_t type_idx = 0;
	if (nwevm->cm.type >= NSS_WIFI_EXT_VDEV_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", nwevm);
		return;
	}

	type_idx = (nwevm->cm.type > NSS_IF_MAX_MSG_TYPES) ?
			(NSS_WIFI_EXT_VDEV_LOG_MESSAGE_TYPE_INDEX(nwevm->cm.type)) : 0;

	nss_info("%px: type[%d]:%s\n", nwevm, nwevm->cm.type, nss_wifi_ext_vdev_log_message_types_str[type_idx]);
	nss_wifi_ext_vdev_log_verbose(nwevm);
}

/*
 * nss_wifi_ext_vdev_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_wifi_ext_vdev_log_rx_msg(struct nss_wifi_ext_vdev_msg *nwevm)
{
	uint32_t type_idx = 0;
	if (nwevm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nwevm);
		return;
	}

	type_idx = (nwevm->cm.type > NSS_IF_MAX_MSG_TYPES) ?
			(NSS_WIFI_EXT_VDEV_LOG_MESSAGE_TYPE_INDEX(nwevm->cm.type)) : 0;

	if (nwevm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nwevm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nwevm, nwevm->cm.type,
			nss_wifi_ext_vdev_log_message_types_str[type_idx],
			nwevm->cm.response, nss_cmn_response_str[nwevm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		nwevm, nwevm->cm.type, nss_wifi_ext_vdev_log_message_types_str[type_idx],
		nwevm->cm.response, nss_cmn_response_str[nwevm->cm.response]);

verbose:
	nss_wifi_ext_vdev_log_verbose(nwevm);
}
