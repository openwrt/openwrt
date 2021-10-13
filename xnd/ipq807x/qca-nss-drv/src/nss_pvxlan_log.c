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
 * nss_pvxlan_log.c
 *	NSS PVXLAN logger file.
 */

#include "nss_core.h"

/*
 * nss_pvxlan_log_message_types_str
 *	PVXLAN message strings
 */
static int8_t *nss_pvxlan_log_message_types_str[NSS_PVXLAN_MSG_TYPE_MAX] __maybe_unused = {
	"PVxLAN Sync Stats",
	"PVxLAN Tunnel Configure Rule",
	"PVxLAN Tunnel Unconfigure Rule",
	"PVxLAN Enable Tunnel",
	"PVxLAN Disable Tunnel",
	"PVxLAN Add MAC rule",
	"PVxLAN Delete MAC rule"
};

/*
 * nss_pvxlan_log_error_response_types_str
 *	Strings for error types for PVXLAN messages
 */
static int8_t *nss_pvxlan_log_error_response_types_str[NSS_PVXLAN_ERROR_MAX] __maybe_unused = {
	"PVXLAN Invalid L3 Protocool",
	"PVXLAN Invalid UDP Protocol",
	"PVXLAN Tunnel Disabled",
	"PVXLAN Tunnel Enabled",
	"PVXLAN Tunnel Not Configured",
	"PVXLAN Invalid IP Node",
	"PVXLAN Invalid Flag",
	"PVXLAN MAC Table Full",
	"PVXLAN MAC Exists",
	"PVXLAN MAC Does Not Exist"
};

/*
 * nss_pvxlan_log_rule_msg()
 *	Log NSS PVXLAN rule message.
 */
static void nss_pvxlan_log_rule_msg(struct nss_pvxlan_rule_msg *npvrm)
{
	nss_trace("%px: NSS PVXLAN Rule message \n"
		"Encap Rule Src IP: %px\n"
		"Encap Rule Src Port: %d\n"
		"Encap Rule Dst Ip: %px\n"
		"Encap Rule Dst Port: %d\n"
		"RPS: %d\n"
		"Flags: %x\n"
		"Tunnel ID: %d\n",
		npvrm,
		&npvrm->encap.src.ip,
		npvrm->encap.src_port,
		&npvrm->encap.dest.ip,
		npvrm->encap.dest_port,
		npvrm->rps, npvrm->flags,
		npvrm->tunnel_id);
}

/*
 * nss_pvxlan_mac_rule_msg()
 *	Log NSS PVxLAN MAC rule message.
 */
static void nss_pvxlan_log_mac_msg(struct nss_pvxlan_mac_msg *npvcm)
{
	nss_trace("%px: NSS PVXLAN MAC message \n"
		"PVxLAN Mac Addr: %x : %x : %x"
		"PVxLAN Flags: %u\n"
		"PVxLAN VNet ID: %u\n"
		"PVxLAN Tunnel ID: %d\n"
		"PVxLAN Policy ID: %d",
		npvcm,
		npvcm->mac_addr[0], npvcm->mac_addr[1],
		npvcm->mac_addr[2], npvcm->flags,
		npvcm->vnet_id, npvcm->tunnel_id,
		npvcm->policy_id);
}

/*
 * nss_pvxlan_log_rule_cfg_msg()
 *	Log NSS PVxLAN rule configure message.
 */
static void nss_pvxlan_log_rule_cfg_msg(struct nss_pvxlan_msg *npvm)
{
	struct nss_pvxlan_rule_msg *npvrm __maybe_unused = &npvm->msg.rule_cfg;
	nss_pvxlan_log_rule_msg(npvrm);
}

/*
 * nss_pvxlan_log_rule_uncfg_msg()
 *	Log NSS PVxLAN rule unconfigure message.
 */
static void nss_pvxlan_log_rule_uncfg_msg(struct nss_pvxlan_msg *npvm)
{
	struct nss_pvxlan_rule_msg *npvrm __maybe_unused = &npvm->msg.rule_uncfg;
	nss_pvxlan_log_rule_msg(npvrm);
}

/*
 * nss_pvxlan_log_enable_msg()
 *	Log NSS PVxLAN rule enable message.
 */
static void nss_pvxlan_log_enable_msg(struct nss_pvxlan_msg *npvm)
{
	struct nss_pvxlan_tunnel_state_msg *npvrm __maybe_unused = &npvm->msg.enable;
	nss_trace("%px: NSS PVXLAN Tunnel state message: Enable \n", npvrm);
}

/*
 * nss_pvxlan_log_disable_msg()
 *	Log NSS PVxLAN rule disable message.
 */
static void nss_pvxlan_log_disable_msg(struct nss_pvxlan_msg *npvm)
{
	nss_trace("%px: NSS PVXLAN Tunnel state message: Disable \n", npvm);
}

/*
 * nss_pvxlan_log_mac_add_msg()
 *	Log NSS PVXLAN mac rule add message.
 */
static void nss_pvxlan_log_mac_add_msg(struct nss_pvxlan_msg *npvm)
{
	struct nss_pvxlan_mac_msg *npvcm __maybe_unused = &npvm->msg.mac_add;
	nss_pvxlan_log_mac_msg(npvcm);
}

/*
 * nss_pvxlan_log_mac_del_msg()
 *	Log NSS PVXLAN mac rule del message.
 */
static void nss_pvxlan_log_mac_del_msg(struct nss_pvxlan_msg *npvm)
{
	struct nss_pvxlan_mac_msg *npvcm __maybe_unused = &npvm->msg.mac_del;
	nss_pvxlan_log_mac_msg(npvcm);
}

/*
 * nss_pvxlan_log_verbose()
 *	Log message contents.
 */
static void nss_pvxlan_log_verbose(struct nss_pvxlan_msg *npvm)
{
	switch (npvm->cm.type) {
	case NSS_PVXLAN_MSG_TYPE_TUNNEL_CREATE_RULE:
		nss_pvxlan_log_rule_cfg_msg(npvm);
		break;

	case NSS_PVXLAN_MSG_TYPE_TUNNEL_DESTROY_RULE:
		nss_pvxlan_log_rule_uncfg_msg(npvm);
		break;

	case NSS_PVXLAN_MSG_TYPE_TUNNEL_ENABLE:
		nss_pvxlan_log_enable_msg(npvm);
		break;

	case NSS_PVXLAN_MSG_TYPE_TUNNEL_DISABLE:
		nss_pvxlan_log_disable_msg(npvm);
		break;

	case NSS_PVXLAN_MSG_TYPE_MAC_ADD:
		nss_pvxlan_log_mac_add_msg(npvm);
		break;

	case NSS_PVXLAN_MSG_TYPE_MAC_DEL:
		nss_pvxlan_log_mac_del_msg(npvm);
		break;

	case NSS_PVXLAN_MSG_TYPE_SYNC_STATS:
		break;

	default:
		nss_trace("%px: Invalid message type\n", npvm);
		break;
	}
}

/*
 * nss_pvxlan_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_pvxlan_log_tx_msg(struct nss_pvxlan_msg *npvm)
{
	if (npvm->cm.type >= NSS_PVXLAN_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", npvm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", npvm, npvm->cm.type, nss_pvxlan_log_message_types_str[npvm->cm.type]);
	nss_pvxlan_log_verbose(npvm);
}

/*
 * nss_pvxlan_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_pvxlan_log_rx_msg(struct nss_pvxlan_msg *npvm)
{
	if (npvm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", npvm);
		return;
	}

	if (npvm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (npvm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", npvm, npvm->cm.type,
			nss_pvxlan_log_message_types_str[npvm->cm.type],
			npvm->cm.response, nss_cmn_response_str[npvm->cm.response]);
		goto verbose;
	}

	if (npvm->cm.error >= NSS_PVXLAN_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			npvm, npvm->cm.type, nss_pvxlan_log_message_types_str[npvm->cm.type],
			npvm->cm.response, nss_cmn_response_str[npvm->cm.response],
			npvm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		npvm, npvm->cm.type, nss_pvxlan_log_message_types_str[npvm->cm.type],
		npvm->cm.response, nss_cmn_response_str[npvm->cm.response],
		npvm->cm.error, nss_pvxlan_log_error_response_types_str[npvm->cm.error]);

verbose:
	nss_pvxlan_log_verbose(npvm);
}
