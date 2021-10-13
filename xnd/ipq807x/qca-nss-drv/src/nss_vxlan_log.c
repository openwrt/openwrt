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
 * nss_vxlan_log.c
 *	NSS VXLAN logger file.
 */

#include "nss_core.h"

/*
 * nss_vxlan_log_message_types_str
 *	VXLAN message strings
 */
static int8_t *nss_vxlan_log_message_types_str[NSS_VXLAN_MSG_TYPE_MAX] __maybe_unused = {
	"VxLAN Sync Stats",
	"VxLAN Tunnel Configure Rule",
	"VxLAN Tunnel Unconfigure Rule",
	"VxLAN Enable Tunnel",
	"VxLAN Disable Tunnel",
	"VxLAN Add MAC rule",
	"VxLAN Delete MAC rule",
	"VxLAN MAC DB Stats"
};

/*
 * nss_vxlan_log_error_response_types_str
 *	Strings for error types for VXLAN messages
 */
static int8_t *nss_vxlan_log_error_response_types_str[NSS_VXLAN_ERROR_TYPE_MAX] __maybe_unused = {
	"VxLAN Unknown Error",
	"VXLAN Decap Register fail",
	"VXLAN Dest IP mismatch",
	"VXLAN Invalid VNI",
	"VXLAN Invalid L3 Proto",
	"VXLAN Invalid UDP Proto",
	"VXLAN Invalid Src Port",
	"VXLAN MAC Bad entry",
	"VXLAN MAC Entry exists",
	"VXLAN MAC Entry does not exist",
	"VXLAN MAC Entry unhashed",
	"VXLAN MAC Entry alloc failed",
	"VXLAN MAC Entry delete failed",
	"VXLAN MAC Table full",
	"VXLAN Sibling Node does not exist",
	"VXLAN Tunnel Configured",
	"VXLAN Tunnel Unconfigured",
	"VXLAN Tunnel addition failed",
	"VXLAN Tunnel Disabled",
	"VXLAN Tunnel Enabled",
	"VXLAN Tunnel Entry exists"
};

/*
 * nss_vxlan_log_rule_msg()
 *	Log NSS VXLAN rule message.
 */
static void nss_vxlan_log_rule_msg(struct nss_vxlan_rule_msg *nvrm)
{
	nss_trace("%px: NSS VXLAN Rule message \n"
		"VxLAN Tunnel Flags: %x\n"
		"VNET ID: %u\n"
		"Flowlabel: %u\n"
		"TOS: %u\n"
		"TTL: %u\n"
		"source port min: %u max: %u"
		"destination port: %u",
		nvrm,
		nvrm->tunnel_flags,
		nvrm->vni,
		nvrm->flow_label,
		nvrm->tos,
		nvrm->ttl,
		nvrm->src_port_min,
		nvrm->src_port_max,
		nvrm->dest_port);
}

/*
 * nss_vxlan_mac_rule_msg()
 *	Log NSS Vxlan MAC rule message.
 */
static void nss_vxlan_log_mac_msg(struct nss_vxlan_mac_msg *nvmm)
{
	nss_trace("%px: NSS VXLAN MAC message \n"
		"Encap Rule Src IP: %px\n"
		"Encap Rule Dst Ip: %px\n"
		"Vxlan VNet ID: %u\n"
		"Vxlan Mac Addr: %pM",
		nvmm,
		&nvmm->encap.src_ip,
		&nvmm->encap.dest_ip,
		nvmm->vni,
		nvmm->mac_addr);
}

/*
 * nss_vxlan_log_rule_create_msg()
 *	Log NSS Vxlan rule create message.
 */
static void nss_vxlan_log_rule_create_msg(struct nss_vxlan_msg *nvm)
{
	struct nss_vxlan_rule_msg *nvrm __maybe_unused = &nvm->msg.vxlan_create;
	nss_vxlan_log_rule_msg(nvrm);
}

/*
 * nss_vxlan_log_rule_destroy_msg()
 *	Log NSS Vxlan rule destroy message.
 */
static void nss_vxlan_log_rule_destroy_msg(struct nss_vxlan_msg *nvm)
{
	struct nss_vxlan_rule_msg *nvrm __maybe_unused = &nvm->msg.vxlan_destroy;
	nss_vxlan_log_rule_msg(nvrm);
}

/*
 * nss_vxlan_log_enable_msg()
 *	Log NSS Vxlan rule enable message.
 */
static void nss_vxlan_log_enable_msg(struct nss_vxlan_msg *nvm)
{
	nss_trace("%px: NSS VXLAN Tunnel state message: Enable \n", nvm);
}

/*
 * nss_vxlan_log_disable_msg()
 *	Log NSS Vxlan rule disable message.
 */
static void nss_vxlan_log_disable_msg(struct nss_vxlan_msg *nvm)
{
	nss_trace("%px: NSS VXLAN Tunnel state message: Disable \n", nvm);
}

/*
 * nss_vxlan_log_mac_add_msg()
 *	Log NSS VXLAN mac rule add message.
 */
static void nss_vxlan_log_mac_add_msg(struct nss_vxlan_msg *nvm)
{
	struct nss_vxlan_mac_msg *nvmm __maybe_unused = &nvm->msg.mac_add;
	nss_vxlan_log_mac_msg(nvmm);
}

/*
 * nss_vxlan_log_mac_del_msg()
 *	Log NSS VXLAN mac rule del message.
 */
static void nss_vxlan_log_mac_del_msg(struct nss_vxlan_msg *nvm)
{
	struct nss_vxlan_mac_msg *nvmm __maybe_unused = &nvm->msg.mac_del;
	nss_vxlan_log_mac_msg(nvmm);
}

/*
 * nss_vxlan_log_verbose()
 *	Log message contents.
 */
static void nss_vxlan_log_verbose(struct nss_vxlan_msg *nvm)
{
	switch (nvm->cm.type) {
	case NSS_VXLAN_MSG_TYPE_TUN_CONFIGURE:
		nss_vxlan_log_rule_create_msg(nvm);
		break;

	case NSS_VXLAN_MSG_TYPE_TUN_UNCONFIGURE:
		nss_vxlan_log_rule_destroy_msg(nvm);
		break;

	case NSS_VXLAN_MSG_TYPE_TUN_ENABLE:
		nss_vxlan_log_enable_msg(nvm);
		break;

	case NSS_VXLAN_MSG_TYPE_TUN_DISABLE:
		nss_vxlan_log_disable_msg(nvm);
		break;

	case NSS_VXLAN_MSG_TYPE_MAC_ADD:
		nss_vxlan_log_mac_add_msg(nvm);
		break;

	case NSS_VXLAN_MSG_TYPE_MAC_DEL:
		nss_vxlan_log_mac_del_msg(nvm);
		break;

	case NSS_VXLAN_MSG_TYPE_STATS_SYNC:
	case NSS_VXLAN_MSG_TYPE_MACDB_STATS:
		break;

	default:
		nss_trace("%px: Invalid message type\n", nvm);
		break;
	}
}

/*
 * nss_vxlan_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_vxlan_log_tx_msg(struct nss_vxlan_msg *nvm)
{
	if (nvm->cm.type >= NSS_VXLAN_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", nvm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nvm, nvm->cm.type, nss_vxlan_log_message_types_str[nvm->cm.type]);
	nss_vxlan_log_verbose(nvm);
}

/*
 * nss_vxlan_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_vxlan_log_rx_msg(struct nss_vxlan_msg *nvm)
{
	if (nvm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nvm);
		return;
	}

	if (nvm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nvm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nvm, nvm->cm.type,
			nss_vxlan_log_message_types_str[nvm->cm.type],
			nvm->cm.response, nss_cmn_response_str[nvm->cm.response]);
		goto verbose;
	}

	if (nvm->cm.error >= NSS_VXLAN_ERROR_TYPE_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nvm, nvm->cm.type, nss_vxlan_log_message_types_str[nvm->cm.type],
			nvm->cm.response, nss_cmn_response_str[nvm->cm.response],
			nvm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nvm, nvm->cm.type, nss_vxlan_log_message_types_str[nvm->cm.type],
		nvm->cm.response, nss_cmn_response_str[nvm->cm.response],
		nvm->cm.error, nss_vxlan_log_error_response_types_str[nvm->cm.error]);

verbose:
	nss_vxlan_log_verbose(nvm);
}
