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
 * nss_gre_tunnel_log.c
 *	NSS GRE Tunnel logger file.
 */

#include "nss_core.h"

/*
 * nss_gre_tunnel_log_message_types_str
 *	NSS GRE Tunnel message strings
 */
static int8_t *nss_gre_tunnel_log_message_types_str[NSS_GRE_TUNNEL_MSG_MAX] __maybe_unused = {
	"GRE Tunnel configure",
	"GRE Tunnel session destroy",
	"GRE Tunnel stats",
	"GRE Tunnel configure DI to WLAN ID",
	"GRE Tunnel message inquiry"
};

/*
 * nss_gre_tunnel_log_configure_msg()
 *	Log NSS GRE Tunnel configure message.
 */
static void nss_gre_tunnel_log_configure_msg(struct nss_gre_tunnel_msg *ngm)
{
	struct nss_gre_tunnel_configure *ngcm __maybe_unused = &ngm->msg.configure;
	nss_trace("%px: NSS GRE Tunnel configure message \n"
		"Meta Header Version: %d\n"
		"GRE Mode: %x\n"
		"IP Type: %x\n"
		"Encryption Type: %d\n"
		"Source Port: %d\n"
		"Destination Port: %d\n"
		"Crypto Node Identifier: %d\n"
		"Encryption Crypto Index: %d\n"
		"Decryption Crypto Index: %d\n"
		"Word0 header: %d\n"
		"Initialization Vector: %px\n"
		"Sibling Interface Number: %d\n"
		"TTL: %d\n"
		"RPS: %d\n"
		"Reserved: %x\n"
		"Word1 Header: %x\n"
		"Word2 Header: %x\n"
		"Word3 Header: %x\n",
		ngcm, ngcm->mh_version, ngcm->gre_mode,
		ngcm->ip_type, ngcm->encrypt_type,
		ngcm->src_port, ngcm->dest_port,
		ngcm->crypto_node_id, ngcm->crypto_idx_encrypt,
		ngcm->crypto_idx_decrypt, ngcm->word0,
		ngcm->iv_val, ngcm->sibling_if,
		ngcm->ttl, ngcm->rps,
		ngcm->reserved, ngcm->word1,
		ngcm->word2, ngcm->word3);

	/*
	 * Continuation of log message. Different identifiers based on ip_type
	 */
	if (ngcm->ip_type == NSS_GRE_TUNNEL_IP_IPV6) {
		nss_trace("Source IP: %pI6\n"
			"Destination IP: %pI6\n",
			ngcm->src_ip, ngcm->dest_ip);
	} else if (ngcm->ip_type == NSS_GRE_TUNNEL_IP_IPV4) {
		nss_trace("Source IP: %pI4\n"
			"Destination IP: %pI4\n",
			ngcm->src_ip, ngcm->dest_ip);
	}
}

/*
 * nss_gre_tunnel_log_di_to_wlan_id_msg()
 *	Log NSS GRE Tunnel Dynamic Interface to WLAN ID message.
 */
static void nss_gre_tunnel_log_di_to_wlan_id_msg(struct nss_gre_tunnel_msg *ngm)
{
	struct nss_gre_tunnel_di_to_wlan_id *ngdm __maybe_unused = &ngm->msg.dtwi;
	nss_trace("%px: NSS GRE Dynamic Interface to WLAN ID message: \n"
		"Dynamic Interface Number: %d\n"
		"WLAN ID: %x\n",
		ngdm, ngdm->dynamic_interface_num,
		ngdm->wlan_id);
}

/*
 * nss_gre_tunnel_log_verbose()
 *	Log message contents.
 */
static void nss_gre_tunnel_log_verbose(struct nss_gre_tunnel_msg *ngm)
{
	switch (ngm->cm.type) {
	case NSS_GRE_TUNNEL_MSG_CONFIGURE:
	case NSS_GRE_TUNNEL_MSG_INQUIRY:
		nss_gre_tunnel_log_configure_msg(ngm);
		break;

	case NSS_GRE_TUNNEL_MSG_CONFIGURE_DI_TO_WLAN_ID:
		nss_gre_tunnel_log_di_to_wlan_id_msg(ngm);
		break;

	case NSS_GRE_TUNNEL_MSG_SESSION_DESTROY:
	case NSS_GRE_TUNNEL_MSG_STATS:
		/*
		 * No log for these valid messages.
		 */
		break;

	default:
		nss_trace("%px: Invalid message type\n", ngm);
		break;
	}
}

/*
 * nss_gre_tunnel_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_gre_tunnel_log_tx_msg(struct nss_gre_tunnel_msg *ngm)
{
	if (ngm->cm.type >= NSS_GRE_TUNNEL_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", ngm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ngm, ngm->cm.type, nss_gre_tunnel_log_message_types_str[ngm->cm.type]);
	nss_gre_tunnel_log_verbose(ngm);
}

/*
 * nss_gre_tunnel_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_gre_tunnel_log_rx_msg(struct nss_gre_tunnel_msg *ngm)
{
	if (ngm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ngm);
		return;
	}

	if (ngm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ngm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ngm, ngm->cm.type,
			nss_gre_tunnel_log_message_types_str[ngm->cm.type],
			ngm->cm.response, nss_cmn_response_str[ngm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		ngm, ngm->cm.type, nss_gre_tunnel_log_message_types_str[ngm->cm.type],
		ngm->cm.response, nss_cmn_response_str[ngm->cm.response]);

verbose:
	nss_gre_tunnel_log_verbose(ngm);
}
