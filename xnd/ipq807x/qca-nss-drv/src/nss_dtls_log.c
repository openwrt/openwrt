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
 * nss_dtls_log.c
 *	NSS DTLS logger file.
 */

#include "nss_core.h"

/*
 * nss_dtls_log_message_types_str
 *	DTLS message strings
 */
static int8_t *nss_dtls_log_message_types_str[NSS_DTLS_MSG_MAX] __maybe_unused = {
	"DTLS Session Configure",
	"DTLS Session Destroy",
	"DTLS Session Stats",
	"DTLS Encap Cipher Update",
	"DTLS Encap Cipher Switch",
	"DTLS Decap Cipher Update",
	"DTLS Decap Cipher Switch"
};

/*
 * nss_dtls_log_error_response_types_str
 *	Strings for error types for DTLS messages
 */
static int8_t *nss_dtls_log_error_response_types_str[NSS_DTLS_ERR_MAX] __maybe_unused = {
	"DTLS Unknown Message",
	"DTLS Invalid APP Interface",
	"DTLS Invalid Parameter",
	"DTLS Invalid Version",
	"DTLS No Memory"
};

/*
 * nss_dtls_session_config_msg()
 *	Log DTLS session configure message.
 */
static void nss_dtls_session_config_msg(struct nss_dtls_msg *ndm)
{
	struct nss_dtls_session_configure *ndscm __maybe_unused = &ndm->msg.cfg;
	nss_trace("%px: NSS DTLS Session Configure Message:\n"
		"DTLS Version: %d\n"
		"DTLS Flags: %x\n"
		"DTLS crypto index encap: %d\n"
		"DTLS crypto index decap: %d\n"
		"DTLS IV length for encapsulation: %d\n"
		"DTLS IV length for decapsulation: %d\n"
		"DTLS authentication hash length for encapsulation: %d\n"
		"DTLS authentication hash length for decapsulation: %d\n"
		"DTLS cipher algorithm for encapsulation: %x\n"
		"DTLS authentication algorithm for encapsulation: %x\n"
		"DTLS cipher algorithm for decapsulation: %x\n"
		"DTLS authentication algorithm for decapsulation: %x\n"
		"DTLS NSS interface: %x\n"
		"DTLS source port: %d\n"
		"DTLS destination port: %d\n"
		"DTLS source ip: %px\n"
		"DTLS destination ip: %px\n"
		"DTLS window size: %d\n"
		"DTLS epoch: %d\n"
		"DTLS outer IP TTL: %d\n"
		"DTLS reserved1 padding: %x\n"
		"DTLS reserved2 padding: %x\n",
		ndscm, ndscm->ver,
		ndscm->flags, ndscm->crypto_idx_encap,
		ndscm->crypto_idx_decap, ndscm->iv_len_encap,
		ndscm->iv_len_decap, ndscm->hash_len_encap,
		ndscm->hash_len_decap, ndscm->cipher_algo_encap,
		ndscm->auth_algo_encap, ndscm->cipher_algo_decap,
		ndscm->auth_algo_decap, ndscm->nss_app_if,
		ndscm->sport, ndscm->dport,
		&ndscm->sip, &ndscm->dip,
		ndscm->window_size, ndscm->epoch,
		ndscm->oip_ttl, ndscm->reserved1,
		ndscm->reserved2);
}

/*
 * nss_dtls_session_cipher_upddate_msg()
 *	Log DTLS Session Cipher Update message.
 */
static void nss_dtls_session_cipher_update_msg(struct nss_dtls_msg *ndm)
{
	struct nss_dtls_session_cipher_update *ndscum __maybe_unused = &ndm->msg.cipher_update;
	nss_trace("%px: NSS DTLS Session Cipher Update message\n"
		"DTLS crypto index: %d\n"
		"DTLS hash length: %d\n"
		"DTLS crypto IV length for encapsulation: %d\n"
		"DTLS encapsulation cipher: %x\n"
		"DTLS encapsulation authentication algorigthm: %x\n"
		"DTLS epoch: %d\n"
		"DTLS reserved: %x\n",
		ndscum, ndscum->crypto_idx,
		ndscum->hash_len, ndscum->iv_len,
		ndscum->cipher_algo, ndscum->auth_algo,
		ndscum->epoch, ndscum->reserved);
}

/*
 * nss_dtls_log_verbose()
 *	Log message contents.
 */
static void nss_dtls_log_verbose(struct nss_dtls_msg *ndm)
{
	switch (ndm->cm.type) {
	case NSS_DTLS_MSG_REKEY_DECAP_CIPHER_UPDATE:
	case NSS_DTLS_MSG_REKEY_ENCAP_CIPHER_UPDATE:
		nss_dtls_session_cipher_update_msg(ndm);
		break;

	case NSS_DTLS_MSG_SESSION_CONFIGURE:
		nss_dtls_session_config_msg(ndm);
		break;

	default:
		nss_warning("%px: Invalid message type\n", ndm);
		break;
	}
}

/*
 * nss_dtls_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_dtls_log_tx_msg(struct nss_dtls_msg *ndm)
{
	if (ndm->cm.type >= NSS_DTLS_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", ndm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ndm, ndm->cm.type, nss_dtls_log_message_types_str[ndm->cm.type]);
	nss_dtls_log_verbose(ndm);
}

/*
 * nss_dtls_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_dtls_log_rx_msg(struct nss_dtls_msg *ndm)
{
	if (ndm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ndm);
		return;
	}

	if (ndm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ndm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ndm, ndm->cm.type,
			nss_dtls_log_message_types_str[ndm->cm.type],
			ndm->cm.response, nss_cmn_response_str[ndm->cm.response]);
		goto verbose;
	}

	if (ndm->cm.error >= NSS_DTLS_ERR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ndm, ndm->cm.type, nss_dtls_log_message_types_str[ndm->cm.type],
			ndm->cm.response, nss_cmn_response_str[ndm->cm.response],
			ndm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ndm, ndm->cm.type, nss_dtls_log_message_types_str[ndm->cm.type],
		ndm->cm.response, nss_cmn_response_str[ndm->cm.response],
		ndm->cm.error, nss_dtls_log_error_response_types_str[ndm->cm.error]);

verbose:
	nss_dtls_log_verbose(ndm);
}
