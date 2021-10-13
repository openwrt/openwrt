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
 * nss_pptp_log.c
 *	NSS PPTP logger file.
 */

#include "nss_core.h"

/*
 * nss_pptp_log_message_types_str
 *	NSS PPTP message strings
 */
static int8_t *nss_pptp_log_message_types_str[NSS_PPTP_MSG_MAX] __maybe_unused = {
	"PPTP Session Configure",
	"PPTP Session Deconfigure",
	"PPTP Stats",
};

/*
 * nss_pptp_log_configure_msg()
 *	Log NSS PPTP Session Configure.
 */
static void nss_pptp_log_configure_msg(struct nss_pptp_msg *npm)
{
	struct nss_pptp_session_configure_msg *npcm __maybe_unused = &npm->msg.session_configure_msg;
	nss_trace("%px: NSS PPTP Session Configure message\n"
		"PPTP Source Call ID: %x\n"
		"PPTP Destination Call ID: %x\n"
		"PPTP Source IP: %pI4\n"
		"PPTP Destination IP: %pI4\n",
		npcm, npcm->src_call_id,
		npcm->dst_call_id, &npcm->sip,
		&npcm->dip);
}

/*
 * nss_pptp_log_deconfigure_msg()
 *	Log NSS PPTP Session Deconfigure.
 */
static void nss_pptp_log_deconfigure_msg(struct nss_pptp_msg *npm)
{
	struct nss_pptp_session_deconfigure_msg *npdm __maybe_unused = &npm->msg.session_deconfigure_msg;
	nss_trace("%px: NSS PPTP Session Configure message \n"
		"PPTP Source Call ID: %x\n",
		npdm, npdm->src_call_id);
}

/*
 * nss_pptp_log_verbose()
 *	Log message contents.
 */
static void nss_pptp_log_verbose(struct nss_pptp_msg *npm)
{
	switch (npm->cm.type) {
	case NSS_PPTP_MSG_SESSION_CONFIGURE:
		nss_pptp_log_configure_msg(npm);
		break;

	case NSS_PPTP_MSG_SESSION_DECONFIGURE:
		nss_pptp_log_deconfigure_msg(npm);
		break;

	case NSS_PPTP_MSG_SYNC_STATS:
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
 * nss_pptp_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_pptp_log_tx_msg(struct nss_pptp_msg *npm)
{
	if (npm->cm.type >= NSS_PPTP_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", npm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", npm, npm->cm.type, nss_pptp_log_message_types_str[npm->cm.type]);
	nss_pptp_log_verbose(npm);
}

/*
 * nss_pptp_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_pptp_log_rx_msg(struct nss_pptp_msg *npm)
{
	if (npm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", npm);
		return;
	}

	if (npm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (npm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", npm, npm->cm.type,
			nss_pptp_log_message_types_str[npm->cm.type],
			npm->cm.response, nss_cmn_response_str[npm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		npm, npm->cm.type, nss_pptp_log_message_types_str[npm->cm.type],
		npm->cm.response, nss_cmn_response_str[npm->cm.response]);

verbose:
	nss_pptp_log_verbose(npm);
}
