/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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
 **************************************************************************
 */

/*
 * nss_clmap_log.c
 *	NSS clmap logger file.
 */

#include "nss_core.h"

/*
 * nss_clmap_log_message_types_str
 *	clmap message strings
 */
static char *nss_clmap_log_message_types_str[NSS_CLMAP_MSG_TYPE_MAX] __maybe_unused = {
	"Clmap sync stats",
	"Clmap enable interface",
	"Clmap disable interface",
	"Clmap add MAC rule",
	"Clmap delete MAC rule",
	"Clmap flush MAC rule"
};

/*
 * nss_clmap_log_error_types_str
 *	Strings for error types for clmap messages
 */
static char *nss_clmap_log_error_types_str[NSS_CLMAP_ERROR_MAX] __maybe_unused = {
	"Clmap unknown error",
	"Clmap interface disabled",
	"Clmap interface enabled",
	"Clmap invalid VLAN",
	"Clmap invalid tunnel ID",
	"Clmap MAC table full",
	"Clmap MAC exists",
	"Clmap MAC does not exist",
	"Clmap MAC entry unhashed",
	"Clmap MAC entry insert failed",
	"Clmap MAC entry alloc failed",
	"Clmap MAC entry delete failed"
};

/*
 * nss_clmap_log_mac_msg()
 *	Log NSS clmap MAC rule message.
 */
static void nss_clmap_log_mac_msg(struct nss_clmap_mac_msg *npvcm)
{
	nss_trace("%px: NSS clmap MAC message \n"
		"Clmap Mac Addr: %x : %x : %x"
		"Clmap Flags: %u\n"
		"Clmap VLAN ID: %u\n"
		"Clmap Next-hop Interface Number: %d\n",
		npvcm,
		npvcm->mac_addr[0], npvcm->mac_addr[1],
		npvcm->mac_addr[2], npvcm->flags,
		npvcm->vlan_id, npvcm->nexthop_ifnum);
}

/*
 * nss_clmap_log_interface_enable_msg()
 *	Log NSS clmap rule enable message.
 */
static void nss_clmap_log_interface_enable_msg(struct nss_clmap_msg *npvm)
{
	nss_trace("%px: NSS clmap interface state message: Enable \n", npvm);
}

/*
 * nss_clmap_log_interface_disable_msg()
 *	Log NSS clmap rule disable message.
 */
static void nss_clmap_log_interface_disable_msg(struct nss_clmap_msg *npvm)
{
	nss_trace("%px: NSS clmap interface state message: Disable \n", npvm);
}

/*
 * nss_clmap_log_mac_add_msg()
 *	Log NSS clmap mac rule add message.
 */
static void nss_clmap_log_mac_add_msg(struct nss_clmap_msg *npvm)
{
	struct nss_clmap_mac_msg *npvcm __maybe_unused = &npvm->msg.mac_add;
	nss_clmap_log_mac_msg(npvcm);
}

/*
 * nss_clmap_log_mac_del_msg()
 *	Log NSS clmap mac rule del message.
 */
static void nss_clmap_log_mac_del_msg(struct nss_clmap_msg *npvm)
{
	struct nss_clmap_mac_msg *npvcm __maybe_unused = &npvm->msg.mac_del;
	nss_clmap_log_mac_msg(npvcm);
}

/*
 * nss_clmap_log_mac_flush_msg()
 *	Log NSS clmap mac rule flush message.
 */
static void nss_clmap_log_mac_flush_msg(struct nss_clmap_msg *npvm)
{
	struct nss_clmap_flush_mac_msg *npvcm __maybe_unused = &npvm->msg.mac_flush;
	nss_trace("%px: NSS clmap MAC flush message \n"
		"Clmap Next-hop Interface Number: %d\n",
		npvcm, npvcm->nexthop_ifnum);
}

/*
 * nss_clmap_log_verbose()
 *	Log message contents.
 */
static void nss_clmap_log_verbose(struct nss_clmap_msg *npvm)
{
	switch (npvm->cm.type) {
	case NSS_CLMAP_MSG_TYPE_INTERFACE_ENABLE:
		nss_clmap_log_interface_enable_msg(npvm);
		break;

	case NSS_CLMAP_MSG_TYPE_INTERFACE_DISABLE:
		nss_clmap_log_interface_disable_msg(npvm);
		break;

	case NSS_CLMAP_MSG_TYPE_MAC_ADD:
		nss_clmap_log_mac_add_msg(npvm);
		break;

	case NSS_CLMAP_MSG_TYPE_MAC_DEL:
		nss_clmap_log_mac_del_msg(npvm);
		break;

	case NSS_CLMAP_MSG_TYPE_MAC_FLUSH:
		nss_clmap_log_mac_flush_msg(npvm);
		break;

	case NSS_CLMAP_MSG_TYPE_SYNC_STATS:
		break;

	default:
		nss_trace("%px: Invalid message type\n", npvm);
		break;
	}
}

/*
 * nss_clmap_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_clmap_log_tx_msg(struct nss_clmap_msg *npvm)
{
	if (npvm->cm.type >= NSS_CLMAP_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", npvm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", npvm, npvm->cm.type, nss_clmap_log_message_types_str[npvm->cm.type]);
	nss_clmap_log_verbose(npvm);
}

/*
 * nss_clmap_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_clmap_log_rx_msg(struct nss_clmap_msg *npvm)
{
	if (npvm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", npvm);
		return;
	}

	if (npvm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (npvm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", npvm, npvm->cm.type,
			nss_clmap_log_message_types_str[npvm->cm.type],
			npvm->cm.response, nss_cmn_response_str[npvm->cm.response]);
		goto verbose;
	}

	if (npvm->cm.error >= NSS_CLMAP_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			npvm, npvm->cm.type, nss_clmap_log_message_types_str[npvm->cm.type],
			npvm->cm.response, nss_cmn_response_str[npvm->cm.response],
			npvm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		npvm, npvm->cm.type, nss_clmap_log_message_types_str[npvm->cm.type],
		npvm->cm.response, nss_cmn_response_str[npvm->cm.response],
		npvm->cm.error, nss_clmap_log_error_types_str[npvm->cm.error]);

verbose:
	nss_clmap_log_verbose(npvm);
}
