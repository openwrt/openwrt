/*
 **************************************************************************
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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
 * nss_tun6rd_log.c
 *	NSS TUN6RD logger file.
 */

#include "nss_core.h"

/*
 * nss_tun6rd_log_message_types_str
 *	NSS TUN6RD message strings
 */
static int8_t *nss_tun6rd_log_message_types_str[NSS_TUN6RD_MAX] __maybe_unused = {
	"TUN6RD Attach PNODE",
	"TUN6RD Stats",
	"TUN6RD Update Peer",
};

/*
 * nss_tun6rd_log_attach_pnode_msg()
 *	Log NSS TUN6RD Attach PNODE
 */
static void nss_tun6rd_log_attach_pnode_msg(struct nss_tun6rd_msg *ntm)
{
	struct nss_tun6rd_attach_tunnel_msg *ntam __maybe_unused = &ntm->msg.tunnel;
	nss_trace("%px: NSS TUN6RD Attach Tunnel message \n"
		"TUN6RD Source Address: %pI4\n"
		"TUN6RD Destination Address: %pI4\n"
		"TUN6RD Type of Service: %d\n"
		"TUN6RD Time To Live: %d\n"
		"TUN6RD Sibling Interface Number: %d\n",
		ntam, &ntam->saddr,
		&ntam->daddr, ntam->tos,
		ntam->ttl, ntam->sibling_if_num);
}

/*
 * nss_tun6rd_log_set_peer_msg()
 *	Log NSS TUN6RD Set Peer Message
 */
static void nss_tun6rd_log_set_peer_msg(struct nss_tun6rd_msg *ntm)
{
	struct nss_tun6rd_set_peer_msg *ntspm __maybe_unused = &ntm->msg.peer;
	nss_trace("%px: NSS TUN6RD Set Peer message \n"
		"TUN6RD IPv6 Address: %pI6\n"
		"TUN6RD Destination: %pI4\n",
		ntspm, ntspm->ipv6_address,
		&ntspm->dest);
}

/*
 * nss_tun6rd_log_verbose()
 *	Log message contents.
 */
static void nss_tun6rd_log_verbose(struct nss_tun6rd_msg *ntm)
{
	switch (ntm->cm.type) {
	case NSS_TUN6RD_ATTACH_PNODE:
		nss_tun6rd_log_attach_pnode_msg(ntm);
		break;

	case NSS_TUN6RD_ADD_UPDATE_PEER:
		nss_tun6rd_log_set_peer_msg(ntm);
		break;

	case NSS_TUN6RD_RX_STATS_SYNC:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_trace("%px: Invalid message type\n", ntm);
		break;
	}
}

/*
 * nss_tun6rd_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_tun6rd_log_tx_msg(struct nss_tun6rd_msg *ntm)
{
	if (ntm->cm.type >= NSS_TUN6RD_MAX) {
		nss_warning("%px: Invalid message type\n", ntm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ntm, ntm->cm.type, nss_tun6rd_log_message_types_str[ntm->cm.type]);
	nss_tun6rd_log_verbose(ntm);
}

/*
 * nss_tun6rd_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_tun6rd_log_rx_msg(struct nss_tun6rd_msg *ntm)
{
	if (ntm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ntm);
		return;
	}

	if (ntm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ntm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ntm, ntm->cm.type,
			nss_tun6rd_log_message_types_str[ntm->cm.type],
			ntm->cm.response, nss_cmn_response_str[ntm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		ntm, ntm->cm.type, nss_tun6rd_log_message_types_str[ntm->cm.type],
		ntm->cm.response, nss_cmn_response_str[ntm->cm.response]);

verbose:
	nss_tun6rd_log_verbose(ntm);
}
