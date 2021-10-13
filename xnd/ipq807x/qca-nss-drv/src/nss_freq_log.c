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
 * nss_freq_log.c
 *	NSS Freq logger file.
 */

#include "nss_core.h"

/*
 * nss_freq_log_message_types_str
 *	Freq message strings
 */
static int8_t *nss_freq_log_message_types_str[COREFREQ_METADATA_TYPE_MAX] __maybe_unused = {
	"Freq Error Message",
	"Freq Change",
	"Freq ACK",
	"TX Core Stats",
};

/*
 * nss_freq_log_msg()
 *	Log NSS Freq message.
 */
static void nss_freq_log_msg(struct nss_corefreq_msg *ncm)
{
	struct nss_freq_msg *nfm __maybe_unused = &ncm->msg.nfc;
	nss_trace("%px: NSS Freq Message:\n"
		"Frequency request: %d\n"
		"Frequency start/end: %d\n"
		"Frequency stats enable: %d\n"
		"Current Frequency: %d\n"
		"Frequency ACK: %d\n",
		nfm, nfm->frequency, nfm->start_or_end,
		nfm->stats_enable, nfm->freq_current,
		nfm->ack);
}

/*
 * nss_freq_log_verbose()
 *	Log message contents.
 */
static void nss_freq_log_verbose(struct nss_corefreq_msg *ncm)
{
	switch (ncm->cm.type) {
	case COREFREQ_METADATA_TYPE_RX_FREQ_CHANGE:
	case COREFREQ_METADATA_TYPE_TX_FREQ_ACK:
		nss_freq_log_msg(ncm);
		break;

	case COREFREQ_METADATA_TYPE_TX_CORE_STATS:
		/*
		 * No log for a valid stats message.
		 */
		break;

	default:
		nss_warning("%px: Invalid message type\n", ncm);
		break;
	}
}

/*
 * nss_freq_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_freq_log_rx_msg(struct nss_corefreq_msg *ncm)
{
	if (ncm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ncm);
		return;
	}

	if (ncm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ncm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d], response[%d]:%s\n", ncm, ncm->cm.type,
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		ncm, ncm->cm.type, nss_freq_log_message_types_str[ncm->cm.type],
		ncm->cm.response, nss_cmn_response_str[ncm->cm.response]);

verbose:
	nss_freq_log_verbose(ncm);
}
