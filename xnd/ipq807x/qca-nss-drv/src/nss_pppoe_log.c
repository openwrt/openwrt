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
 * nss_pppoe_log.c
 *	NSS PPPOE logger file.
 */

#include "nss_core.h"

/*
 * nss_pppoe_log_message_types_str
 *	NSS PPPOE message strings
 */
static int8_t *nss_pppoe_log_message_types_str[NSS_PPPOE_MSG_MAX] __maybe_unused = {
	"PPPOE Session Create",
	"PPPOE Session Destroy",
	"PPPOE Stats",
};

/*
 * nss_pppoe_log_session_create_msg()
 *	Log NSS Session Create.
 */
static void nss_pppoe_log_session_create_msg(struct nss_pppoe_msg *npm)
{
	struct nss_pppoe_create_msg *npcm __maybe_unused = &npm->msg.create;
	nss_trace("%px: NSS PPPOE Session Create message \n"
		"PPPOE Base Interface Number: %d\n"
		"PPPOE MTU: %d\n"
		"PPPOE Server MAC: %pM\n"
		"PPPOE Local MAC: %pM\n"
		"PPPOE Session ID: %d\n",
		npcm, npcm->base_if_num,
		npcm->mtu, npcm->server_mac,
		npcm->local_mac, npcm->session_id);
}

/*
 * nss_pppoe_log_session_destroy_msg()
 *	Log NSS Session Destroy.
 */
static void nss_pppoe_log_session_destroy_msg(struct nss_pppoe_msg *npm)
{
	struct nss_pppoe_destroy_msg *npdm __maybe_unused = &npm->msg.destroy;
	nss_trace("%px: NSS PPPOE Session Destroy message \n"
		"PPPOE Session ID: %d\n"
		"PPPOE Server MAC: %pM\n"
		"PPPOE Local MAC: %pM\n",
		npdm, npdm->session_id,
		npdm->server_mac, npdm->local_mac);
}

/*
 * nss_pppoe_log_verbose()
 *	Log message contents.
 */
static void nss_pppoe_log_verbose(struct nss_pppoe_msg *npm)
{
	switch (npm->cm.type) {
	case NSS_PPPOE_MSG_SESSION_CREATE:
		nss_pppoe_log_session_create_msg(npm);
		break;

	case NSS_PPPOE_MSG_SESSION_DESTROY:
		nss_pppoe_log_session_destroy_msg(npm);
		break;

	case NSS_PPPOE_MSG_SYNC_STATS:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_trace("%px: Invalid message type\n", npm);
		break;
	}
}

/*
 * nss_pppoe_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_pppoe_log_tx_msg(struct nss_pppoe_msg *npm)
{
	if (npm->cm.type >= NSS_PPPOE_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", npm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", npm, npm->cm.type, nss_pppoe_log_message_types_str[npm->cm.type]);
	nss_pppoe_log_verbose(npm);
}

/*
 * nss_pppoe_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_pppoe_log_rx_msg(struct nss_pppoe_msg *npm)
{
	if (npm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", npm);
		return;
	}

	if (npm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (npm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", npm, npm->cm.type,
			nss_pppoe_log_message_types_str[npm->cm.type],
			npm->cm.response, nss_cmn_response_str[npm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		npm, npm->cm.type, nss_pppoe_log_message_types_str[npm->cm.type],
		npm->cm.response, nss_cmn_response_str[npm->cm.response]);

verbose:
	nss_pppoe_log_verbose(npm);
}
