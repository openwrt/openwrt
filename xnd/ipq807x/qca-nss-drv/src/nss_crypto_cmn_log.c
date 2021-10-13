/*
 **************************************************************************
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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
 * nss_crypto_cmn_log.c
 *	NSS Crypto Common logger file.
 */

#include "nss_core.h"

/*
 * nss_crypto_cmn_log_message_types_str
 *	Crypto Common message strings
 */
static int8_t *nss_crypto_cmn_log_message_types_str[NSS_CRYPTO_CMN_MSG_TYPE_MAX] __maybe_unused = {
	"Crypto Common Invalid Message",
	"Crypto Common CRYPTO CMN Initialize Node",
	"Crypto Common Initialize Engine",
	"Crypto Common Initialize DMA Pair",
	"Crypto Common Update Context Information",
	"Crypto Common Clear Context Information",
	"Crypto Common Verify Context Active",
	"Crypto Common Synchronous Node Statistics"
	"Crypto Common Synchronouts Engine Statistics",
	"Crypto Common Synchronous Context Statistics"
};

/*
 * nss_crypto_cmn_log_error_response_types_str
 *	Strings for error types for crypto common messages
 */
static int8_t *nss_crypto_cmn_log_error_response_types_str[NSS_CRYPTO_CMN_MSG_ERROR_MAX] __maybe_unused = {
	"Crypto Common No Error",
	"Crypto Common Header Version Not Supported",
	"Crypto Common Context Index out-of-range for node",
	"Crypto Common DMA mask out-of-range",
	"Crypto Common DMA count exceeds Token",
	"Crypto Common Token Allocation failed",
	"Crypto Common Context Index out-of-range",
	"Crypto Common Context has references",
	"Crypto Common Bad Context Size",
	"Crypto Common Bad Algorithm",
	"Crypto Common Context Allocation failed",
	"Crypto Common Context has no references",
	"Crypto Common Invalid Context Flags"
};

/*
 * nss_crypto_cmn_node_msg()
 *	Log NSS crypto common node message.
 */
static void nss_crypto_cmn_node_msg(struct nss_crypto_cmn_msg *ncm)
{
	struct nss_crypto_cmn_node *ncnm __maybe_unused = &ncm->msg.node;
	nss_trace("%px: NSS crypto common node message:\n"
		"Crypto Common Max DMA Rings: %d\n"
		"Crypto Common Max Contex: %d\n"
		"Crypto Common Max Context Size: %d\n",
		ncnm, ncnm->max_dma_rings,
		ncnm->max_ctx, ncnm->max_ctx_size);
}

/*
 * nss_crypto_cmn_engine_msg()
 *	Log NSS crypto cmn engine message.
 */
static void nss_crypto_cmn_engine_msg(struct nss_crypto_cmn_msg *ncm)
{
	struct nss_crypto_cmn_engine *ncem __maybe_unused = &ncm->msg.eng;
	nss_trace("%px: NSS crypto common engine message \n"
		"Crypto Common Firmware Version: %px\n"
		"Crypto Common DMA Mask: %x\n"
		"Crypto Common Token Count: %d\n",
		ncem, &ncem->fw_ver,
		ncem->dma_mask, ncem->req_count);
}

/*
 * nss_crypto_cmn_dma_msg()
 *	Log NSS crypto cmn dma message.
 */
static void nss_crypto_cmn_dma_msg(struct nss_crypto_cmn_msg *ncm)
{
	struct nss_crypto_cmn_dma *ncdm __maybe_unused = &ncm->msg.dma;
	nss_trace("%px: NSS crypto common dma message \n"
		"Crypto Common DMA Pair ID: %d\n",
		ncdm, ncdm->pair_id);
}

/*
 * nss_crypto_cmn_ctx_msg()
 *	Log NSS crypto cmn context message.
 */
static void nss_crypto_cmn_ctx_msg(struct nss_crypto_cmn_msg *ncm)
{
	struct nss_crypto_cmn_ctx *nccm __maybe_unused = &ncm->msg.ctx;
	nss_trace("%px: NSS crypto common context message \n"
		"Crypto Common Context Spare Words: %px\n"
		"Crypto Common Index: %d\n"
		"Crypto Common Secure Offset: %d\n"
		"Crypto Common Cipher Key: %px\n"
		"Crypto Common Authorization Key: %px\n"
		"Crypto Common Nonce Value: %px\n"
		"Crypto Common Algorithm: %x\n"
		"Crypto Common Context Specific Flags: %x\n",
		nccm, &nccm->spare,
		nccm->index, nccm->sec_offset,
		&nccm->cipher_key, &nccm->auth_key,
		&nccm->nonce, nccm->algo, nccm->flags);
}

/*
 * nss_crypto_cmn_log_verbose()
 *	Log message contents.
 */
static void nss_crypto_cmn_log_verbose(struct nss_crypto_cmn_msg *ncm)
{
	switch (ncm->cm.type) {
	case NSS_CRYPTO_CMN_MSG_TYPE_SETUP_NODE:
		nss_crypto_cmn_node_msg(ncm);
		break;

	case NSS_CRYPTO_CMN_MSG_TYPE_SETUP_ENG:
		nss_crypto_cmn_engine_msg(ncm);
		break;

	case NSS_CRYPTO_CMN_MSG_TYPE_SETUP_DMA:
		nss_crypto_cmn_dma_msg(ncm);
		break;

	case NSS_CRYPTO_CMN_MSG_TYPE_SETUP_CTX:
	case NSS_CRYPTO_CMN_MSG_TYPE_CLEAR_CTX:
	case NSS_CRYPTO_CMN_MSG_TYPE_VERIFY_CTX:
		nss_crypto_cmn_ctx_msg(ncm);
		break;

	case NSS_CRYPTO_CMN_MSG_TYPE_SYNC_NODE_STATS:
	case NSS_CRYPTO_CMN_MSG_TYPE_SYNC_ENG_STATS:
	case NSS_CRYPTO_CMN_MSG_TYPE_SYNC_CTX_STATS:
		/* Getting logged in stats */
		break;

	default:
		nss_warning("%px: Invalid message type\n", ncm);
		break;
	}
}

/*
 * nss_crypto_cmn_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_crypto_cmn_log_tx_msg(struct nss_crypto_cmn_msg *ncm)
{
	if (ncm->cm.type >= NSS_CRYPTO_CMN_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", ncm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ncm, ncm->cm.type, nss_crypto_cmn_log_message_types_str[ncm->cm.type]);
	nss_crypto_cmn_log_verbose(ncm);
}

/*
 * nss_crypto_cmn_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_crypto_cmn_log_rx_msg(struct nss_crypto_cmn_msg *ncm)
{
	if (ncm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ncm);
		return;
	}

	if (ncm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ncm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ncm, ncm->cm.type,
			nss_crypto_cmn_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response]);
		goto verbose;
	}

	if (ncm->cm.error >= NSS_CRYPTO_CMN_MSG_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ncm, ncm->cm.type, nss_crypto_cmn_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
			ncm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ncm, ncm->cm.type, nss_crypto_cmn_log_message_types_str[ncm->cm.type],
		ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
		ncm->cm.error, nss_crypto_cmn_log_error_response_types_str[ncm->cm.error]);

verbose:
	nss_crypto_cmn_log_verbose(ncm);
}
