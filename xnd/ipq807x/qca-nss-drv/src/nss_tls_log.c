/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE
 **************************************************************************
 */

/*
 * nss_tls_log.c
 *	NSS TLS logger file.
 */

#include "nss_core.h"

/*
 * nss_tls_log_message_types_str
 *	TLS message strings
 */
static int8_t *nss_tls_log_message_types_str[NSS_TLS_MSG_MAX] __maybe_unused = {
	"TLS Node Configure",
	"TLS Context Configure",
	"TLS Context Deconfigure",
	"TLS Cipher Update",
	"TLS Context Sync",
	"TLS Node Sync",
};

/*
 * nss_tls_log_error_response_types_str
 *	Strings for error types for TLS messages
 */
static int8_t *nss_tls_log_error_response_types_str[NSS_TLS_ERROR_MAX] __maybe_unused = {
	"TLS no_error",
	"TLS unknown message",
	"TLS fail node already config",
	"TLS fail inner ctx",
	"TLS fail outer ctx",
	"TLS fail req pool",
	"TLS invalid block len",
	"TLS invalid hash len",
	"TLS invalid version",
	"TLS invalid context words",
	"TLS fail alloc hwctx",
	"TLS fail copy ctx",
	"TLS Invalid algorithm",
	"TLS fail nomem"
};

/*
 * nss_tls_node_config_msg()
 *	Log TLS node configure message.
 */
static void nss_tls_node_config_msg(struct nss_tls_msg *ntm)
{
	nss_trace("%px: NSS TLS Node Configure Message:\n"
			"TLS Interface: %d\n", ntm, ntm->cm.interface);
}

/*
 * nss_tls_ctx_config_msg()
 *	Log TLS session configure message.
 */
static void nss_tls_ctx_config_msg(struct nss_tls_msg *ntm)
{
	struct nss_tls_ctx_config *ntccm __maybe_unused = &ntm->msg.ctx_cfg;
	nss_trace("%px: NSS TLS Context Configure Message:\n"
		"TLS Except if_num: %d\n",
		ntccm, ntccm->except_ifnum);
}

/*
 * nss_tls_cipher_upddate_msg()
 *	Log TLS Cipher Update message.
 */
static void nss_tls_cipher_update_msg(struct nss_tls_msg *ntm)
{
	struct nss_tls_cipher_update *ntcum __maybe_unused = &ntm->msg.cipher_update;
	nss_trace("%px: NSS TLS Cipher Update message\n"
		"TLS crypto index: %d\n",
		ntcum, ntcum->crypto_idx);
}

/*
 * nss_tls_log_verbose()
 *	Log message contents.
 */
static void nss_tls_log_verbose(struct nss_tls_msg *ntm)
{
	switch (ntm->cm.type) {
	case NSS_TLS_MSG_TYPE_NODE_CONFIG:
		nss_tls_node_config_msg(ntm);
		break;

	case NSS_TLS_MSG_TYPE_CIPHER_UPDATE:
		nss_tls_cipher_update_msg(ntm);
		break;

	case NSS_TLS_MSG_TYPE_CTX_CONFIG:
		nss_tls_ctx_config_msg(ntm);
		break;

	default:
		nss_warning("%px: Invalid message type\n", ntm);
		break;
	}
}

/*
 * nss_tls_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_tls_log_tx_msg(struct nss_tls_msg *ntm)
{
	if (ntm->cm.type >= NSS_TLS_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", ntm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ntm, ntm->cm.type, nss_tls_log_message_types_str[ntm->cm.type]);
	nss_tls_log_verbose(ntm);
}

/*
 * nss_tls_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_tls_log_rx_msg(struct nss_tls_msg *ntm)
{
	if (ntm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ntm);
		return;
	}

	if (ntm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ntm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ntm, ntm->cm.type,
			nss_tls_log_message_types_str[ntm->cm.type],
			ntm->cm.response, nss_cmn_response_str[ntm->cm.response]);
		goto verbose;
	}

	if (ntm->cm.error >= NSS_TLS_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ntm, ntm->cm.type, nss_tls_log_message_types_str[ntm->cm.type],
			ntm->cm.response, nss_cmn_response_str[ntm->cm.response],
			ntm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ntm, ntm->cm.type, nss_tls_log_message_types_str[ntm->cm.type],
		ntm->cm.response, nss_cmn_response_str[ntm->cm.response],
		ntm->cm.error, nss_tls_log_error_response_types_str[ntm->cm.error]);

verbose:
	nss_tls_log_verbose(ntm);
}
