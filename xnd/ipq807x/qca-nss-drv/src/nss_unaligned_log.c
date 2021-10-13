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
 * nss_unaligned_log.c
 *	NSS unaligned logger file.
 */

#include "nss_core.h"

/*
 * nss_unaligned_log_message_types_str
 *	NSS unaligned message strings
 */
static int8_t *nss_unaligned_log_message_types_str[NSS_UNALIGNED_MSG_MAX] __maybe_unused = {
	"Unaligned Stats Message",
};

/*
 * nss_unaligned_log_verbose()
 *	Log message contents.
 */
static void nss_unaligned_log_verbose(struct nss_unaligned_msg *um)
{
	switch (um->cm.type) {
	case NSS_UNALIGNED_MSG_STATS:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_trace("%px: Invalid message type\n", um);
		break;
	}
}

/*
 * nss_unaligned_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_unaligned_log_rx_msg(struct nss_unaligned_msg *um)
{
	if (um->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", um);
		return;
	}

	if (um->cm.response == NSS_CMN_RESPONSE_NOTIFY || (um->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", um, um->cm.type,
			nss_unaligned_log_message_types_str[um->cm.type],
			um->cm.response, nss_cmn_response_str[um->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		um, um->cm.type, nss_unaligned_log_message_types_str[um->cm.type],
		um->cm.response, nss_cmn_response_str[um->cm.response]);

verbose:
	nss_unaligned_log_verbose(um);
}
