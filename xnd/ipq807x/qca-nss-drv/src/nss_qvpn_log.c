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
 * nss_qvpn_log.c
 *	NSS qvpn logger file.
 */

#include "nss_core.h"

/*
 * nss_qvpn_log_message_types_str
 *	qvpn message strings
 */
static int8_t *nss_qvpn_log_message_types_str[NSS_QVPN_MSG_TYPE_MAX] __maybe_unused = {
	"QVPN tunnel config",
	"QVPN tunnel deconfig",
	"QVPN crypto key add",
	"QVPN crypto key delete",
	"QVPN crypto crypto key activate",
	"QVPN crypto key Deactivate",
	"QVPN statistics synchronization"
};

/*
 * nss_qvpn_log_error_response_types_str
 *	Strings for error types for qvpn messages
 */
static int8_t *nss_qvpn_log_error_response_types_str[NSS_QVPN_ERROR_TYPE_MAX] __maybe_unused = {
	"QVPN No error",
	"QVPN Unknown message",
	"QVPN Tunnel already configured",
	"QVPN Invalid interface",
	"QVPN Invalid sibling interface number",
	"QVPN Invalid IV size",
	"QVPN Invalid HMAC size",
	"QVPN Invalid crypto block size",
	"QVPN Invalid session idx size",
	"QVPN Supported processing command count invalid",
	"QVPN L4 protocol encapsulation is not supported",
	"QVPN Invalid sibling interface type",
	"QVPN Total number of commands is invalid",
	"QVPN Entry not found",
	"QVPN Entry not active",
	"QVPN Entry already active",
	"QVPN Invalid crypto index",
	"QVPN Key info allocation failure",
	"QVPN Invalid command profile",
	"QVPN VPN with tail not supported"
};

/*
 * nss_qvpn_tun_config_msg()
 *	Log NSS QVPN configuration message.
 */
static void nss_qvpn_log_tun_config_msg(struct nss_qvpn_msg *ncm)
{
	struct nss_qvpn_tunnel_config_msg *nqtcm __maybe_unused = &ncm->msg.tunnel_config;
	nss_trace("%px: NSS QVPN tunnel config message \n"
		"Sibling interface: %d"
		"Total number of commands: %d"
		"Commands: %px"
		"Source IP: %x:%x:%x:%x\n"
		"Source Port: %d\n"
		"Destination IP:  %x:%x:%x:%x\n"
		"Destination Port: %d\n"
		"Header Flags: %x\n"
		"Sequence number size: %d\n"
		"Sequence number offset: %d\n"
		"Anti-replay algorithm: %d\n"
		"Session ID size: %d\n"
		"Session ID offset: %x\n"
		"VPN header head size: %d\n"
		"VPN header head offset: %d\n"
		"VPN header tail size: %d\n"
		"VPN header head: %px\n"
		"VPN header tail: %px\n",
		nqtcm,
		nqtcm->sibling_if,
		nqtcm->total_cmds,
		nqtcm->cmd,
		nqtcm->hdr_cfg.src_ip[0], nqtcm->hdr_cfg.src_ip[1], nqtcm->hdr_cfg.src_ip[2], nqtcm->hdr_cfg.src_ip[3],
		nqtcm->hdr_cfg.src_port,
		nqtcm->hdr_cfg.dst_ip[0], nqtcm->hdr_cfg.dst_ip[1], nqtcm->hdr_cfg.dst_ip[2], nqtcm->hdr_cfg.dst_ip[3],
		nqtcm->hdr_cfg.dst_port,
		nqtcm->hdr_cfg.hdr_flags,
		nqtcm->hdr_cfg.seqnum_size,
		nqtcm->hdr_cfg.seqnum_offset,
		nqtcm->hdr_cfg.anti_replay_alg,
		nqtcm->hdr_cfg.session_id_size,
		nqtcm->hdr_cfg.session_id_offset,
		nqtcm->hdr_cfg.vpn_hdr_head_size,
		nqtcm->hdr_cfg.vpn_hdr_head_offset,
		nqtcm->hdr_cfg.vpn_hdr_tail_size,
		nqtcm->hdr_cfg.vpn_hdr_head,
		nqtcm->hdr_cfg.vpn_hdr_tail);
}

/*
 * nss_qvpn_log_tun_deconfig_msg()
 *	Log NSS qvpn tunnel deconfigure message.
 */
static void nss_qvpn_log_tun_deconfig_msg(struct nss_qvpn_msg *ncm)
{
	nss_trace("%px: NSS QVPN deconfigure message \n", ncm);
}

/*
 * nss_qvpn_log_crypto_key_add_msg()
 *	Log NSS QVPN crypto key add message.
 */
static void nss_qvpn_log_crypto_key_add_msg(struct nss_qvpn_msg *ncm)
{
	struct nss_qvpn_crypto_key_add_msg *nqckam __maybe_unused = &ncm->msg.key_add;
	nss_trace("%px: NSS QVPN crypto key add message \n"
		"Crypto index: %d\n"
		"Crypto session ID: %px",
		nqckam,
		nqckam->crypto_idx,
		nqckam->session_id);
}

/*
 * nss_qvpn_log_crypto_key_activate_msg()
 *	Log NSS QVPN crypto key activate message.
 */
static void nss_qvpn_log_crypto_key_activate_msg(struct nss_qvpn_msg *ncm)
{
	struct nss_qvpn_crypto_key_activate_msg *nqckam __maybe_unused = &ncm->msg.key_activate;
	nss_trace("%px: NSS QVPN crypto key activate message \n"
		"Crypto index: %d\n"
		"Crypto VPN header head: %px",
		nqckam,
		nqckam->crypto_idx,
		nqckam->vpn_hdr_head);
}

/*
 * nss_qvpn_log_crypto_key_del_msg()
 *	Log NSS QVPN crypto key delete message.
 */
static void nss_qvpn_log_crypto_key_del_msg(struct nss_qvpn_msg *ncm)
{
	struct nss_qvpn_crypto_key_del_msg *nqckdm __maybe_unused = &ncm->msg.key_del;
	nss_trace("%px: NSS QVPN crypto key delete message \n"
		"Crypto index: %d\n",
		nqckdm,
		nqckdm->crypto_idx);
}

/*
 * nss_qvpn_log_crypto_key_deactivate_msg()
 *	Log NSS QVPN crypto key deactivate message.
 */
static void nss_qvpn_log_crypto_key_deactivate_msg(struct nss_qvpn_msg *ncm)
{
	struct nss_qvpn_crypto_key_del_msg *nqckdm __maybe_unused = &ncm->msg.key_del;
	nss_trace("%px: NSS QVPN crypto key deactivate message \n"
		"Crypto index: %d\n",
		nqckdm,
		nqckdm->crypto_idx);
}

/*
 * nss_qvpn_log_verbose()
 *	Log message contents.
 */
static void nss_qvpn_log_verbose(struct nss_qvpn_msg *ncm)
{
	switch (ncm->cm.type) {
	case NSS_QVPN_MSG_TYPE_TUNNEL_CONFIGURE:
		nss_qvpn_log_tun_config_msg(ncm);
		break;

	case NSS_QVPN_MSG_TYPE_TUNNEL_DECONFIGURE:
		nss_qvpn_log_tun_deconfig_msg(ncm);
		break;

	case NSS_QVPN_MSG_TYPE_CRYPTO_KEY_ADD:
		nss_qvpn_log_crypto_key_add_msg(ncm);
		break;

	case NSS_QVPN_MSG_TYPE_CRYPTO_KEY_ACTIVATE:
		nss_qvpn_log_crypto_key_activate_msg(ncm);
		break;

	case NSS_QVPN_MSG_TYPE_CRYPTO_KEY_DEL:
		nss_qvpn_log_crypto_key_del_msg(ncm);
		break;

	case NSS_QVPN_MSG_TYPE_CRYPTO_KEY_DEACTIVATE:
		nss_qvpn_log_crypto_key_deactivate_msg(ncm);
		break;

	default:
		nss_trace("%px: Invalid message type\n", ncm);
		break;
	}
}

/*
 * nss_qvpn_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_qvpn_log_tx_msg(struct nss_qvpn_msg *ncm)
{
	if (ncm->cm.type >= NSS_QVPN_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", ncm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ncm, ncm->cm.type, nss_qvpn_log_message_types_str[ncm->cm.type]);
	nss_qvpn_log_verbose(ncm);
}

/*
 * nss_qvpn_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_qvpn_log_rx_msg(struct nss_qvpn_msg *ncm)
{
	if (ncm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ncm);
		return;
	}

	if (ncm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ncm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ncm, ncm->cm.type,
			nss_qvpn_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response]);
		goto verbose;
	}

	if (ncm->cm.error >= NSS_QVPN_ERROR_TYPE_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ncm, ncm->cm.type, nss_qvpn_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
			ncm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ncm, ncm->cm.type, nss_qvpn_log_message_types_str[ncm->cm.type],
		ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
		ncm->cm.error, nss_qvpn_log_error_response_types_str[ncm->cm.error]);

verbose:
	nss_qvpn_log_verbose(ncm);
}
