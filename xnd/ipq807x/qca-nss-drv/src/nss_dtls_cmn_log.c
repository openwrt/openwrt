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
 * nss_dtls_cmn_log.c
 *	NSS DTLS common logger file.
 */

#include "nss_core.h"

/*
 * nss_dtls_cmn_log_message_types_str
 *	DTLS common message strings
 */
static int8_t *nss_dtls_cmn_log_message_types_str[NSS_DTLS_CMN_MSG_MAX] __maybe_unused = {
	"DTLS_CMN Configure Node",
	"DTLS_CMN Configure Base Context Parameter",
	"DTLS_CMN Configure DTLS Parameters",
	"DTLS_CMN Switch DTLS Transform",
	"DTLS_CMN Deconfigure Context",
	"DTLS_CMN Synchronize Stats",
	"DTLS_CMN Node Statistics"
};

/*
 * nss_dtls_cmn_log_error_response_types_str
 *	Strings for error types for DTLS common messages
 */
static int8_t *nss_dtls_cmn_log_error_response_types_str[NSS_DTLS_CMN_ERROR_MAX] __maybe_unused = {
	"DTLS_CMN No Error",
	"DTLS_CMN Unknown MEssage",
	"DTLS_CMN Invalid Destination Interface",
	"DTLS_CMN Invalid Source Interface",
	"DTLS_CMN Invalid Crypto",
	"DTLS_CMN Invalid Version",
	"DTLS_CMN Invalid Context Type",
	"DTLS_CMN Invalid Context Words",
	"DTLS_CMN Hardware Context Alloc Fail",
	"DTLS_CMN Copy Context Failure",
	"DTLS_CMN Switch Hardware Context Fail",
	"DTLS_CMN Already Configured",
	"DTLS_CMN No Memory",
	"DTLS_CMN Copy Nonce Failure"
};

/*
 * nss_dtls_cmn_hdr_config_msg()
 *	Log DTLS common header configure message.
 */
static void nss_dtls_cmn_hdr_config_msg(struct nss_dtls_cmn_msg *ndm)
{
	struct nss_dtls_cmn_ctx_config_hdr *ndchm __maybe_unused = &ndm->msg.hdr_cfg;
	nss_trace("%px: NSS DTLS_CMN Header Configure Message:\n"
		"DTLS_CMN flags: %x\n"
		"DTLS_CMN destination interface number: %d\n"
		"DTLS_CMN source interface number: %d\n"
		"DTLS_CMN source ip: %px\n"
		"DTLS_CMN destination ip: %px\n"
		"DTLS_CMN source port: %d\n"
		"DTLS_CMN destination port: %d\n"
		"DTLS_CMN time to live: %d\n"
		"DTLS_CMN dscp value: %x\n"
		"DTLS_CMN dscp copy value: %x\n"
		"DTLS_CMN DF flag: %x\n",
		ndchm, ndchm->flags,
		ndchm->dest_ifnum, ndchm->src_ifnum,
		&ndchm->sip, &ndchm->dip,
		ndchm->sport, ndchm->dport,
		ndchm->hop_limit_ttl, ndchm->dscp,
		ndchm->dscp_copy, ndchm->df);
};

/*
 * nss_dtls_cmn_dtls_config_msg()
 *	Log DTLS common dtls configure message.
 */
static void nss_dtls_cmn_dtls_config_msg(struct nss_dtls_cmn_msg *ndm)
{
	struct nss_dtls_cmn_ctx_config_dtls *ndcdm __maybe_unused = &ndm->msg.dtls_cfg;
	nss_trace("%px: NSS DTLS_CMN DTLS Configure Message:\n"
		"DTLS_CMN version: %d\n"
		"DTLS_CMN crypto Index: %d\n"
		"DTLS_CMN window size: %d\n"
		"DTLS_CMN initial epoch: %d\n"
		"DTLS_CMN IV length for encapsulation: %d\n"
		"DTLS_CMN authentication hash length for encapsulation: %d\n"
		"DTLS_CMN cipher block length: %d\n"
		"DTLS_CMN reserved: %x\n",
		ndcdm, ndcdm->ver,
		ndcdm->crypto_idx, ndcdm->window_size,
		ndcdm->epoch, ndcdm->iv_len,
		ndcdm->hash_len, ndcdm->blk_len,
		ndcdm->res1);
};

/*
 * nss_dtls_cmn_log_verbose()
 *	Log message contents.
 */
static void nss_dtls_cmn_log_verbose(struct nss_dtls_cmn_msg *ndm)
{
	switch (ndm->cm.type) {
	case NSS_DTLS_CMN_MSG_TYPE_CONFIGURE_HDR:
		nss_dtls_cmn_hdr_config_msg(ndm);
		break;

	case NSS_DTLS_CMN_MSG_TYPE_CONFIGURE_DTLS:
		nss_dtls_cmn_dtls_config_msg(ndm);
		break;

	default:
		nss_warning("%px: Invalid message type\n", ndm);
		break;
	}
}

/*
 * nss_dtls_cmn_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_dtls_cmn_log_tx_msg(struct nss_dtls_cmn_msg *ndm)
{
	if (ndm->cm.type >= NSS_DTLS_CMN_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", ndm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ndm, ndm->cm.type, nss_dtls_cmn_log_message_types_str[ndm->cm.type]);
	nss_dtls_cmn_log_verbose(ndm);
}

/*
 * nss_dtls_cmn_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_dtls_cmn_log_rx_msg(struct nss_dtls_cmn_msg *ndm)
{
	if (ndm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ndm);
		return;
	}

	if (ndm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ndm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ndm, ndm->cm.type,
			nss_dtls_cmn_log_message_types_str[ndm->cm.type],
			ndm->cm.response, nss_cmn_response_str[ndm->cm.response]);
		goto verbose;
	}

	if (ndm->cm.error >= NSS_DTLS_CMN_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ndm, ndm->cm.type, nss_dtls_cmn_log_message_types_str[ndm->cm.type],
			ndm->cm.response, nss_cmn_response_str[ndm->cm.response],
			ndm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ndm, ndm->cm.type, nss_dtls_cmn_log_message_types_str[ndm->cm.type],
		ndm->cm.response, nss_cmn_response_str[ndm->cm.response],
		ndm->cm.error, nss_dtls_cmn_log_error_response_types_str[ndm->cm.error]);

verbose:
	nss_dtls_cmn_log_verbose(ndm);
}
