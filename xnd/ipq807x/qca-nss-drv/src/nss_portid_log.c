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
 * nss_portid_log.c
 *	NSS PORTID logger file.
 */

#include "nss_core.h"

/*
 * nss_portid_log_message_types_str
 *	NSS PORTID message strings
 */
static int8_t *nss_portid_log_message_types_str[NSS_PORTID_MAX_MSG_TYPE] __maybe_unused = {
	"PORTID Configure",
	"PORTID Unconfigure",
	"PORTID Stats",
};

/*
 * nss_portid_log_configure_msg()
 *	Log NSS PORTID Configure.
 */
static void nss_portid_log_configure_msg(struct nss_portid_msg *npm)
{
	struct nss_portid_configure_msg *npcm __maybe_unused = &npm->msg.configure;
	nss_trace("%px: NSS PORTID Configure message \n"
		"PORTID Interface Number: %d\n"
		"PORTID Interface ID: %d\n"
		"PORTID GMAC ID: %d\n",
		npcm, npcm->port_if_num,
		npcm->port_id, npcm->gmac_id);
}

/*
 * nss_portid_log_unconfigure_msg()
 *	Log NSS PORTID Unconfigure.
 */
static void nss_portid_log_unconfigure_msg(struct nss_portid_msg *npm)
{
	struct nss_portid_unconfigure_msg *npum __maybe_unused = &npm->msg.unconfigure;
	nss_trace("%px: NSS PORTID Configure message \n"
		"PORTID Interface Number: %d\n"
		"PORTID Interface ID: %d\n",
		npum, npum->port_if_num,
		npum->port_id);
}

/*
 * nss_portid_log_verbose()
 *	Log message contents.
 */
static void nss_portid_log_verbose(struct nss_portid_msg *npm)
{
	switch (npm->cm.type) {
	case NSS_PORTID_CONFIGURE_MSG:
		nss_portid_log_configure_msg(npm);
		break;

	case NSS_PORTID_UNCONFIGURE_MSG:
		nss_portid_log_unconfigure_msg(npm);
		break;

	case NSS_PORTID_STATS_SYNC_MSG:
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
 * nss_portid_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_portid_log_tx_msg(struct nss_portid_msg *npm)
{
	if (npm->cm.type >= NSS_PORTID_MAX_MSG_TYPE) {
		nss_warning("%px: Invalid message type\n", npm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", npm, npm->cm.type, nss_portid_log_message_types_str[npm->cm.type]);
	nss_portid_log_verbose(npm);
}

/*
 * nss_portid_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_portid_log_rx_msg(struct nss_portid_msg *npm)
{
	if (npm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", npm);
		return;
	}

	if (npm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (npm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", npm, npm->cm.type,
			nss_portid_log_message_types_str[npm->cm.type],
			npm->cm.response, nss_cmn_response_str[npm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		npm, npm->cm.type, nss_portid_log_message_types_str[npm->cm.type],
		npm->cm.response, nss_cmn_response_str[npm->cm.response]);

verbose:
	nss_portid_log_verbose(npm);
}
