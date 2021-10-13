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
 * nss_l2tpv2_log.c
 *	NSS L2TPV2 logger file.
 */

#include "nss_core.h"

/*
 * nss_l2tpv2_log_message_types_str
 *	NSS L2TPV2 message strings
 */
static int8_t *nss_l2tpv2_log_message_types_str[NSS_L2TPV2_MSG_MAX] __maybe_unused = {
	"L2TPV2 Sesstion Create",
	"L2TPV2 Session Destroy",
	"L2TPV2 Stats",
};

/*
 * nss_l2tpv2_log_session_create_msg()
 *	Log NSS L2TPV2 Session Create.
 */
static void nss_l2tpv2_log_session_create_msg(struct nss_l2tpv2_msg *nlm)
{
	struct nss_l2tpv2_session_create_msg *nlcm __maybe_unused = &nlm->msg.session_create_msg;
	nss_trace("%px: NSS L2TPV2 Session Create message \n"
		"L2TPV2 Local Tunnel ID: %x\n"
		"L2TPV2 Local Session ID: %x\n"
		"L2TPV2 Peer Tunnel ID: %x\n"
		"L2TPV2 Peer Session ID: %x\n"
		"L2TPV2 Source IP: %x\n"
		"L2TPV2 Destnation IP: %x\n"
		"L2TPV2 Reorder Timeout: %d\n"
		"L2TPV2 Source Port: %d\n"
		"L2TPV2 Destination Port: %d\n"
		"L2TPV2 Received Sequence Number: %d\n"
		"L2TPV2 Outer IP Packet TTL: %d\n"
		"L2TPV2 UDP Checksum: %d\n",
		nlcm, nlcm->local_tunnel_id,
		nlcm->local_session_id, nlcm->peer_tunnel_id,
		nlcm->peer_session_id, nlcm->sip,
		nlcm->dip, nlcm->reorder_timeout,
		nlcm->sport, nlcm->dport,
		nlcm->recv_seq, nlcm->oip_ttl,
		nlcm->udp_csum);
}

/*
 * nss_l2tpv2_log_session_destroy_msg()
 *	Log NSS L2TPV2 Session Create.
 */
static void nss_l2tpv2_log_session_destroy_msg(struct nss_l2tpv2_msg *nlm)
{
	struct nss_l2tpv2_session_destroy_msg *nldm __maybe_unused = &nlm->msg.session_destroy_msg;
	nss_trace("%px: NSS L2TPV2 Session Destroy message \n"
		"L2TPV2 Local Tunnel ID: %x\n"
		"L2TPV2 Local Session ID: %x\n",
		nldm, nldm->local_tunnel_id,
		nldm->local_session_id);
}

/*
 * nss_l2tpv2_log_verbose()
 *	Log message contents.
 */
static void nss_l2tpv2_log_verbose(struct nss_l2tpv2_msg *nlm)
{
	switch (nlm->cm.type) {
	case NSS_L2TPV2_MSG_SESSION_CREATE:
		nss_l2tpv2_log_session_create_msg(nlm);
		break;

	case NSS_L2TPV2_MSG_SESSION_DESTROY:
		nss_l2tpv2_log_session_destroy_msg(nlm);
		break;

	case NSS_L2TPV2_MSG_SYNC_STATS:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_trace("%px: Invalid message type\n", nlm);
		break;
	}
}

/*
 * nss_l2tpv2_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_l2tpv2_log_tx_msg(struct nss_l2tpv2_msg *nlm)
{
	if (nlm->cm.type >= NSS_L2TPV2_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", nlm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nlm, nlm->cm.type, nss_l2tpv2_log_message_types_str[nlm->cm.type]);
	nss_l2tpv2_log_verbose(nlm);
}

/*
 * nss_l2tpv2_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_l2tpv2_log_rx_msg(struct nss_l2tpv2_msg *nlm)
{
	if (nlm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nlm);
		return;
	}

	if (nlm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nlm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nlm, nlm->cm.type,
			nss_l2tpv2_log_message_types_str[nlm->cm.type],
			nlm->cm.response, nss_cmn_response_str[nlm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		nlm, nlm->cm.type, nss_l2tpv2_log_message_types_str[nlm->cm.type],
		nlm->cm.response, nss_cmn_response_str[nlm->cm.response]);

verbose:
	nss_l2tpv2_log_verbose(nlm);
}
