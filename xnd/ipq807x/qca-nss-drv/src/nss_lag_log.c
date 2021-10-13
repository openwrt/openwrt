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
 * nss_lag_log.c
 *	NSS LAG logger file.
 */

#include "nss_core.h"

/*
 * nss_lag_log_message_types_str
 *	NSS LAG message strings
 */
static int8_t *nss_lag_log_message_types_str[NSS_TX_METADATA_LAG_MAX] __maybe_unused = {
	"LAG State Change",
};

/*
 * nss_lag_log_state_change_msg()
 *	Log NSS LAG State Change.
 */
static void nss_lag_log_state_change_msg(struct nss_lag_msg *nlm)
{
	struct nss_lag_state_change *nlcm __maybe_unused = &nlm->msg.state;
	nss_trace("%px: NSS LAG State Change message \n"
		"LAG ID: %x\n"
		"LAG Interface: %x\n"
		"LAG Event: %d\n",
		nlcm, nlcm->lagid,
		nlcm->interface, nlcm->event);
}

/*
 * nss_lag_log_verbose()
 *	Log message contents.
 */
static void nss_lag_log_verbose(struct nss_lag_msg *nlm)
{
	switch (nlm->cm.type) {
	case NSS_TX_METADATA_LAG_STATE_CHANGE:
		nss_lag_log_state_change_msg(nlm);
		break;

	default:
		nss_trace("%px: Invalid message type\n", nlm);
		break;
	}
}

/*
 * nss_lag_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_lag_log_tx_msg(struct nss_lag_msg *nlm)
{
	if (nlm->cm.type >= NSS_TX_METADATA_LAG_MAX) {
		nss_warning("%px: Invalid message type\n", nlm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nlm, nlm->cm.type, nss_lag_log_message_types_str[nlm->cm.type]);
	nss_lag_log_verbose(nlm);
}

/*
 * nss_lag_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_lag_log_rx_msg(struct nss_lag_msg *nlm)
{
	if (nlm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nlm);
		return;
	}

	if (nlm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nlm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nlm, nlm->cm.type,
			nss_lag_log_message_types_str[nlm->cm.type],
			nlm->cm.response, nss_cmn_response_str[nlm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		nlm, nlm->cm.type, nss_lag_log_message_types_str[nlm->cm.type],
		nlm->cm.response, nss_cmn_response_str[nlm->cm.response]);

verbose:
	nss_lag_log_verbose(nlm);
}
