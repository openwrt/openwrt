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
 * nss_crypto_log.c
 *	NSS Crypto logger file.
 */

#include "nss_core.h"

/*
 * nss_crypto_log_message_types_str
 *	Crypto message strings
 */
static int8_t *nss_crypto_log_message_types_str[NSS_CRYPTO_MSG_TYPE_MAX] __maybe_unused = {
	"Crypto Invalid Message",
	"Crypto Open Engine Message",
	"Crypto Close Engine Message",
	"Crypto Update Session",
	"Crypto Stats Sync",
};

/*
 * nss_crypto_log_error_response_types_str
 *	Strings for error types for CRYPTO messages
 */
static int8_t *nss_crypto_log_error_response_types_str[NSS_CRYPTO_MSG_ERROR_MAX] __maybe_unused = {
	"Crypto No Error",
	"Crypto Invalid Engine",
	"Crypto Unsupported Operation",
	"Crypto Invalid Operation",
	"Crypto Invalid Index Range",
	"Crypto Index Alloc Failure",
};

/*
 * nss_crypto_config_eng_msg()
 *	Log NSS Crypto config engine message.
 */
static void nss_crypto_config_eng_msg(struct nss_crypto_msg *ncm)
{
	struct nss_crypto_config_eng *nccem __maybe_unused = &ncm->msg.eng;
	nss_trace("%px: NSS Crypto Config Engine Message:\n"
		"Crypto Engine Number: %d\n"
		"Crypto BAM Physical Base Address: %x\n"
		"Crypto Physical Base Address: %x\n"
		"Crypto Pipe Description Address: %px\n"
		"Crypto Session Indices: %px\n",
		nccem, nccem->eng_id,
		nccem->bam_pbase, nccem->crypto_pbase,
		&nccem->desc_paddr, &nccem->idx);
}

/*
 * nss_crypto_config_session_msg()
 *	Log NSS Crypto config session message.
 */
static void nss_crypto_config_session_msg(struct nss_crypto_msg *ncm)
{
	struct nss_crypto_config_session *nccsm __maybe_unused = &ncm->msg.session;
	nss_trace("%px: NSS Crypto Config Session message \n"
		"Crypto Session Index: %d\n"
		"Crypto Session State: %d\n"
		"Crypto Session Initialization Vector Length: %d\n",
		nccsm, nccsm->idx,
		nccsm->state, nccsm->iv_len);
}

/*
 * nss_crypto_log_verbose()
 *	Log message contents.
 */
static void nss_crypto_log_verbose(struct nss_crypto_msg *ncm)
{
	switch (ncm->cm.type) {
	case NSS_CRYPTO_MSG_TYPE_OPEN_ENG:
		nss_crypto_config_eng_msg(ncm);
		break;

	case NSS_CRYPTO_MSG_TYPE_UPDATE_SESSION:
		nss_crypto_config_session_msg(ncm);
		break;

	default:
		nss_warning("%px: Invalid message type\n", ncm);
		break;
	}
}

/*
 * nss_crypto_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_crypto_log_tx_msg(struct nss_crypto_msg *ncm)
{
	if (ncm->cm.type >= NSS_CRYPTO_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", ncm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ncm, ncm->cm.type, nss_crypto_log_message_types_str[ncm->cm.type]);
	nss_crypto_log_verbose(ncm);
}

/*
 * nss_crypto_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_crypto_log_rx_msg(struct nss_crypto_msg *ncm)
{
	if (ncm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ncm);
		return;
	}

	if (ncm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ncm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ncm, ncm->cm.type,
			nss_crypto_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response]);
		goto verbose;
	}

	if (ncm->cm.error >= NSS_CRYPTO_MSG_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ncm, ncm->cm.type, nss_crypto_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
			ncm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ncm, ncm->cm.type, nss_crypto_log_message_types_str[ncm->cm.type],
		ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
		ncm->cm.error, nss_crypto_log_error_response_types_str[ncm->cm.error]);

verbose:
	nss_crypto_log_verbose(ncm);
}
