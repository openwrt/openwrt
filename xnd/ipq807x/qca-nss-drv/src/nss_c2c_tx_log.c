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
 * nss_c2c_tx_log.c
 *	NSS C2C TX logger file.
 */

#include "nss_core.h"

/*
 * nss_c2c_tx_log_message_types_str
 *	C2C TX message strings
 */
static int8_t *nss_c2c_tx_log_message_types_str[NSS_C2C_TX_MSG_TYPE_MAX] __maybe_unused = {
	"C2C TX Stats message",
	"C2C TX Map Message",
};

/*
 * nss_c2c_tx_log_error_response_types_str
 *	Strings for error types for c2c_tx messages
 */
static int8_t *nss_c2c_tx_log_error_response_types_str[NSS_C2C_TX_MSG_ERROR_MAX] __maybe_unused = {
	"No error",
	"Invalid Operation"
};

/*
 * nss_c2c_tx_map_msg()()
 *	Log NSS C2C TX Map message.
 */
static void nss_c2c_tx_map_msg(struct nss_c2c_tx_msg *nctm)
{
	struct nss_c2c_tx_map *nctmm __maybe_unused = &nctm->msg.map;
	nss_trace("%px: NSS C2C TX Map message: \n"
		"C2C Receiver Queue Start Address: %d\n"
		"C2C Interrupt Register Address: %d\n",
		nctm,
		nctmm->tx_map, nctmm->c2c_intr_addr);
}

/*
 * nss_c2c_tx_log_verbose()
 *	Log message contents.
 */
static void nss_c2c_tx_log_verbose(struct nss_c2c_tx_msg *nctm)
{
	switch (nctm->cm.type) {
	case NSS_C2C_TX_MSG_TYPE_TX_MAP:
		nss_c2c_tx_map_msg(nctm);
		break;

	default:
		nss_trace("%px: Invalid message type\n", nctm);
		break;
	}
}

/*
 * nss_c2c_tx_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_c2c_tx_log_tx_msg(struct nss_c2c_tx_msg *nctm)
{
	if (nctm->cm.type >= NSS_C2C_TX_MSG_TYPE_MAX) {
		nss_info("%px: Invalid message type\n", nctm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nctm, nctm->cm.type, nss_c2c_tx_log_message_types_str[nctm->cm.type]);
	nss_c2c_tx_log_verbose(nctm);
}

/*
 * nss_c2c_tx_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_c2c_tx_log_rx_msg(struct nss_c2c_tx_msg *nctm)
{
	if (nctm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nctm);
		return;
	}

	if (nctm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nctm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nctm, nctm->cm.type,
			nss_c2c_tx_log_message_types_str[nctm->cm.type],
			nctm->cm.response, nss_cmn_response_str[nctm->cm.response]);
		goto verbose;
	}

	if (nctm->cm.error >= NSS_C2C_TX_MSG_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nctm, nctm->cm.type, nss_c2c_tx_log_message_types_str[nctm->cm.type],
			nctm->cm.response, nss_cmn_response_str[nctm->cm.response],
			nctm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nctm, nctm->cm.type, nss_c2c_tx_log_message_types_str[nctm->cm.type],
		nctm->cm.response, nss_cmn_response_str[nctm->cm.response],
		nctm->cm.error, nss_c2c_tx_log_error_response_types_str[nctm->cm.error]);

verbose:
	nss_c2c_tx_log_verbose(nctm);
}
