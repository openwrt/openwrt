/*
 **************************************************************************
 * Copyright (c) 2018, 2020, The Linux Foundation. All rights reserved.
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
 * nss_capwap_log.c
 *	NSS CAPWAP logger file.
 */

#include "nss_core.h"

/*
 * nss_capwap_log_message_types_str
 *	CAPWAP message strings
 */
static int8_t *nss_capwap_log_message_types_str[NSS_CAPWAP_MSG_TYPE_MAX] __maybe_unused = {
	"No Message",
	"CAPWAP config Rule",
	"CAPWAP unconfig Rule",
	"CAPWAP Enable Tunnel",
	"CAPWAP Disable Tunnel",
	"CAPWAP Update Path MTU",
	"CAPWAP Sync Stats",
	"CAPWAP Version",
	"CAPWAP DTLS",
	"CAPWAP Add Flow Rule",
	"CAPWAP Delete Flow Rule"
};

/*
 * nss_capwap_log_error_response_types_str
 *	Strings for error types for CAPWAP messages
 */
static int8_t *nss_capwap_log_error_response_types_str[NSS_CAPWAP_ERROR_MSG_MAX] __maybe_unused = {
	"CAPWAP Invalid Reassembly Timeout",
	"CAPWAP Invalid PAth MTU",
	"CAPWAP Invalid Max Fragment",
	"CAPWAP Invalid Buffer Size",
	"CAPWAP Invalid L3 Protocool",
	"CAPWAP Invalid UDP Protocol",
	"CAPWAP Invalid Version",
	"CAPWAP Tunnel Disabled",
	"CAPWAP Tunnel Enabled",
	"CAPWAP Tunnel Not Configured",
	"CAPWAP Invalid IP Node",
	"CAPWAP Invalid Type Flag",
	"CAPWAP Inavlid DTLS Config",
	"CAPWAP Flow Table Full",
	"CAPWAP Flow Exists",
	"CAPWAP Flow Does Not Exist"
};

/*
 * nss_capwap_rule_msg()
 *	Log NSS CAPWAP stats message.
 */
static void nss_capwap_rule_msg(struct nss_capwap_msg *ncm)
{
	struct nss_capwap_rule_msg *ncrm __maybe_unused = &ncm->msg.rule;
	nss_trace("%px: NSS CAPWAP Rule message \n"
		"Encap Rule Src IP: %px\n"
		"Encap Rule Src Port: %d\n"
		"Encap Rule Dst Ip: %px\n"
		"Encap Rule Dst Port: %d\n"
		"Encap Rule Path MTU: %d\n"
		"Decap Rule Reassembly Timeout: %d\n"
		"Decap Rule Max Fragments: %d\n"
		"Decap Rule Max Buffer Size: %d\n"
		"Stats Timer: %d\n"
		"RPS: %d\n"
		"Type Flags: %x\n"
		"L3 Protocol: %d\n"
		"UDP Protocol: %d\n"
		"MTU: %d\n"
		"GMAC Interface Number: %d\n"
		"Enabled Features: %x\n"
		"DTLS Interface Number: %d\n"
		"BSSID: %px\n"
		"Outer Segment Value: %x\n",
		ncrm,
		&ncrm->encap.src_ip.ip,
		ncrm->encap.src_port,
		&ncrm->encap.dest_ip.ip,
		ncrm->encap.dest_port,
		ncrm->encap.path_mtu,
		ncrm->decap.reassembly_timeout,
		ncrm->decap.max_fragments,
		ncrm->decap.max_buffer_size,
		ncrm->stats_timer,
		ncrm->rps, ncrm->type_flags,
		ncrm->l3_proto, ncrm->which_udp,
		ncrm->mtu_adjust, ncrm->gmac_ifnum,
		ncrm->enabled_features,
		ncrm->dtls_inner_if_num,
		&ncrm->bssid, ncrm->outer_sgt_value);
}

/*
 * nss_capwap_path_mtu_msg()
 *	Log NSS CAPWAP path MTU message.
 */
static void nss_capwap_path_mtu_msg(struct nss_capwap_msg *ncm)
{
	struct nss_capwap_path_mtu_msg *ncpmm __maybe_unused = &ncm->msg.mtu;
	nss_trace("%px: NSS CAPWAP Path MTU message \n"
		"CAPWAP Path MTU: %d\n",
		ncpmm,
		ncpmm->path_mtu);
}

/*
 * nss_capwap_version_msg()
 *	Log NSS CAPWAP version message.
 */
static void nss_capwap_version_msg(struct nss_capwap_msg *ncm)
{
	struct nss_capwap_version_msg *ncvm __maybe_unused = &ncm->msg.version;
	nss_trace("%px: NSS CAPWAP Version message \n"
		"CAPWAP Version: %d\n",
		ncvm,
		ncvm->version);
}

/*
 * nss_capwap_dtls_msg()
 *	Log NSS CAPWAP dtls message.
 */
static void nss_capwap_dtls_msg(struct nss_capwap_msg *ncm)
{
	struct nss_capwap_dtls_msg *ncdm __maybe_unused = &ncm->msg.dtls;
	nss_trace("%px: NSS CAPWAP dtls message \n"
		"CAPWAP DTLS Enable: %d\n"
		"CAPWAP DTLS Inner Interface Number: %d\n"
		"CAPWAP MTU Adjust: %d\n"
		"CAPWAP Reserved: %x\n",
		ncdm,
		ncdm->enable, ncdm->dtls_inner_if_num,
		ncdm->mtu_adjust, ncdm->reserved);
}

/*
 * nss_capwap_flow_rule_msg()
 *	Log NSS CAPWAP flow rule message.
 */
static void nss_capwap_flow_rule_msg(struct nss_capwap_flow_rule_msg *ncfrm)
{
	nss_trace("%px: NSS CAPWAP Flow Rule message \n"
		"CAPWAP IP Version: %d\n"
		"CAPWAP Layer 4 Protocol: %d\n"
		"CAPWAP Source Port: %d\n"
		"CAPWAP Destination Port: %d\n"
		"CAPWAP Source IP: %x %x %x %x\n"
		"CAPWAP Destination IP: %x %x %x %x"
		"CAPWAP Flow ID: %d",
		ncfrm,
		ncfrm->ip_version, ncfrm->protocol,
		ncfrm->src_port, ncfrm->dst_port,
		ncfrm->src_ip[0], ncfrm->src_ip[1],
		ncfrm->src_ip[2], ncfrm->src_ip[3],
		ncfrm->dst_ip[0], ncfrm->dst_ip[1],
		ncfrm->dst_ip[2], ncfrm->dst_ip[3],
		ncfrm->flow_id);
}

/*
 * nss_capwap_flow_rule_add_msg()
 *	Log NSS CAPWAP flow rule add message.
 */
static void nss_capwap_flow_rule_add_msg(struct nss_capwap_msg *ncm)
{
	struct nss_capwap_flow_rule_msg *ncfrm __maybe_unused = &ncm->msg.flow_rule_add;
	nss_capwap_flow_rule_msg(ncfrm);
}

/*
 * nss_capwap_flow_rule_del_msg()
 *	Log NSS CAPWAP flow rule del message.
 */
static void nss_capwap_flow_rule_del_msg(struct nss_capwap_msg *ncm)
{
	struct nss_capwap_flow_rule_msg *ncfrm __maybe_unused = &ncm->msg.flow_rule_del;
	nss_capwap_flow_rule_msg(ncfrm);
}

/*
 * nss_capwap_log_verbose()
 *	Log message contents.
 */
static void nss_capwap_log_verbose(struct nss_capwap_msg *ncm)
{
	switch (ncm->cm.type) {
	case NSS_CAPWAP_MSG_TYPE_CFG_RULE:
		nss_capwap_rule_msg(ncm);
		break;

	case NSS_CAPWAP_MSG_TYPE_UPDATE_PATH_MTU:
		nss_capwap_path_mtu_msg(ncm);
		break;

	case NSS_CAPWAP_MSG_TYPE_VERSION:
		nss_capwap_version_msg(ncm);
		break;

	case NSS_CAPWAP_MSG_TYPE_DTLS:
		nss_capwap_dtls_msg(ncm);
		break;

	case NSS_CAPWAP_MSG_TYPE_FLOW_RULE_ADD:
		nss_capwap_flow_rule_add_msg(ncm);
		break;

	case NSS_CAPWAP_MSG_TYPE_FLOW_RULE_DEL:
		nss_capwap_flow_rule_del_msg(ncm);
		break;

	default:
		nss_trace("%px: Invalid message type\n", ncm);
		break;
	}
}

/*
 * nss_capwap_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_capwap_log_tx_msg(struct nss_capwap_msg *ncm)
{
	if (ncm->cm.type >= NSS_CAPWAP_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", ncm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ncm, ncm->cm.type, nss_capwap_log_message_types_str[ncm->cm.type]);
	nss_capwap_log_verbose(ncm);
}

/*
 * nss_capwap_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_capwap_log_rx_msg(struct nss_capwap_msg *ncm)
{
	if (ncm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ncm);
		return;
	}

	if (ncm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ncm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ncm, ncm->cm.type,
			nss_capwap_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response]);
		goto verbose;
	}

	if (ncm->cm.error >= NSS_CAPWAP_ERROR_MSG_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ncm, ncm->cm.type, nss_capwap_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
			ncm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ncm, ncm->cm.type, nss_capwap_log_message_types_str[ncm->cm.type],
		ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
		ncm->cm.error, nss_capwap_log_error_response_types_str[ncm->cm.error]);

verbose:
	nss_capwap_log_verbose(ncm);
}
