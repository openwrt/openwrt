/*
 **************************************************************************
 * Copyright (c) 2018, 2020 The Linux Foundation. All rights reserved.
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
 * nss_trustsec_tx_log.c
 *	NSS TRUSTSEC_TX logger file.
 */

#include "nss_core.h"

/*
 * nss_trustsec_tx_log_message_types_str
 *	TRUSTSEC_TX message strings
 */
static int8_t *nss_trustsec_tx_log_message_types_str[NSS_TRUSTSEC_TX_MSG_MAX] __maybe_unused = {
	"TRUSTSEC_TX Configure Message",
	"TRUSTSEC_TX Unconfigure Message",
	"TRUSTSEC_TX Stats Sync",
	"TRUSTSEC_TX Update next Hop",
};

/*
 * nss_trustsec_tx_log_error_response_types_str
 *	Strings for error types for TRUSTSEC_TX messages
 */
static int8_t *nss_trustsec_tx_log_error_response_types_str[NSS_TRUSTSEC_TX_ERR_UNKNOWN] __maybe_unused = {
	"TRUSTSEC_TX Invalid Source Interface",
	"TRUSTSEC_TX Reconfigure Source Interface"
	"TRUSTSEC_TX Destination Interface Not Found",
	"TRUSTSEC_TX Not Configured",
	"TRUSTSEC_TX SGT Mismatch",
	"TRUSTSEC_TX Unknown Error",
};

/*
 * nss_trustsec_tx_log_configure_msg()
 *	Log NSS TRUSTSEC_TX configure message.
 */
static void nss_trustsec_tx_log_configure_msg(struct nss_trustsec_tx_msg *ntm)
{
	struct nss_trustsec_tx_configure_msg *ntcm __maybe_unused = &ntm->msg.configure;
	nss_trace("%px: NSS TRUSTSEC_TX Configure Message:\n"
		"TRUSTSEC_TX Source: %d\n"
		"TRUSTSEC_TX Destination: %d\n"
		"TRUSTSEC_TX Security Group Tag: %d\n",
		ntcm, ntcm->src,
		ntcm->dest, ntcm->sgt);
}

/*
 * nss_trustsec_tx_log_unconfigure_msg()
 *	Log NSS TRUSTSEC_TX unconfigure message.
 */
static void nss_trustsec_tx_log_unconfigure_msg(struct nss_trustsec_tx_msg *ntm)
{
	struct nss_trustsec_tx_unconfigure_msg *ntcm __maybe_unused = &ntm->msg.unconfigure;
	nss_trace("%px: NSS TRUSTSEC_TX Unconfigure Message:\n"
		"TRUSTSEC_TX Source: %d\n"
		"TRUSTSEC_TX Security Group Tag: %d\n",
		ntcm, ntcm->src, ntcm->sgt);
}

/*
 * nss_trustsec_tx_log_update_nexthop_msg()
 *	Log NSS TRUSTSEC_TX update nexthop message.
 */
static void nss_trustsec_tx_log_update_nexthop_msg(struct nss_trustsec_tx_msg *ntm)
{
	struct nss_trustsec_tx_update_nexthop_msg *ntunm __maybe_unused = &ntm->msg.upd_nexthop;
	nss_trace("%px: NSS TRUSTSEC_TX Update Next Hop Message:\n"
		"TRUSTSEC_TX Source: %d\n"
		"TRUSTSEC_TX Destination: %d\n"
		"TRUSTSEC_TX Security Group Tag: %d\n",
		ntunm, ntunm->src,
		ntunm->dest, ntunm->sgt);
}

/*
 * nss_trustsec_tx_log_verbose()
 *	Log message contents.
 */
static void nss_trustsec_tx_log_verbose(struct nss_trustsec_tx_msg *ntm)
{
	switch (ntm->cm.type) {
	case NSS_TRUSTSEC_TX_MSG_CONFIGURE:
		nss_trustsec_tx_log_configure_msg(ntm);
		break;

	case NSS_TRUSTSEC_TX_MSG_UNCONFIGURE:
		nss_trustsec_tx_log_unconfigure_msg(ntm);
		break;

	case NSS_TRUSTSEC_TX_MSG_UPDATE_NEXTHOP:
		nss_trustsec_tx_log_update_nexthop_msg(ntm);
		break;

	case NSS_TRUSTSEC_TX_MSG_STATS_SYNC:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_warning("%px: Invalid message type\n", ntm);
		break;
	}
}

/*
 * nss_trustsec_tx_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_trustsec_tx_log_tx_msg(struct nss_trustsec_tx_msg *ntm)
{
	if (ntm->cm.type >= NSS_TRUSTSEC_TX_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", ntm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ntm, ntm->cm.type, nss_trustsec_tx_log_message_types_str[ntm->cm.type]);
	nss_trustsec_tx_log_verbose(ntm);
}

/*
 * nss_trustsec_tx_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_trustsec_tx_log_rx_msg(struct nss_trustsec_tx_msg *ntm)
{
	if (ntm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ntm);
		return;
	}

	if (ntm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ntm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ntm, ntm->cm.type,
			nss_trustsec_tx_log_message_types_str[ntm->cm.type],
			ntm->cm.response, nss_cmn_response_str[ntm->cm.response]);
		goto verbose;
	}

	if (ntm->cm.error >= NSS_TRUSTSEC_TX_ERR_UNKNOWN) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ntm, ntm->cm.type, nss_trustsec_tx_log_message_types_str[ntm->cm.type],
			ntm->cm.response, nss_cmn_response_str[ntm->cm.response],
			ntm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ntm, ntm->cm.type, nss_trustsec_tx_log_message_types_str[ntm->cm.type],
		ntm->cm.response, nss_cmn_response_str[ntm->cm.response],
		ntm->cm.error, nss_trustsec_tx_log_error_response_types_str[ntm->cm.error]);

verbose:
	nss_trustsec_tx_log_verbose(ntm);
}
