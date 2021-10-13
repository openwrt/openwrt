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
 * nss_dynamic_interface_log.c
 *	NSS Dynamic Interface logger file.
 */

#include "nss_core.h"

/*
 * nss_dynamic_interface_log_message_types_str
 *	Dynamic Interface message strings
 */
static int8_t *nss_dynamic_interface_log_message_types_str[NSS_DYNAMIC_INTERFACE_MAX] __maybe_unused = {
	"Dynamic Interface Alloc Node",
	"Dynamic Interface Dealloc Node"
};

/*
 * nss_dynamic_interface_log_error_response_types_str
 *	Strings for error types for dynamic interface messages
 */
static int8_t *nss_dynamic_interface_log_error_response_types_str[NSS_DYNAMIC_INTERFACE_ERR_MAX] __maybe_unused = {
	"Dynamic Interface Error Unknown Interface",
	"Dynamic Interface Error Unavailable Interface",
	"Dynamic Interface Error Invalid Interface Type",
	"Dynamic Interface Error Invalid Interface Number",
	"Dynamic Interface Error Alloc Function Unavailable",
	"Dynamic Interface Error Dealloc Funciton Unavailable",
	"Dynamic Interface Error Allocation Error",
	"Dynamic Interface Error Interface Number Mismatch"
};

/*
 * nss_dynamic_interface_alloc_node_msg()
 *	Log Dynamic Interface alloc node message.
 */
static void nss_dynamic_interface_alloc_node_log_msg(struct nss_dynamic_interface_msg *ndm)
{
	struct nss_dynamic_interface_alloc_node_msg *ndanm __maybe_unused = &ndm->msg.alloc_node;
	nss_trace("%px: NSS Dynamic Interface Alloc Node Message:\n"
		"Dynamic Interface Type: %d\n"
		"Dynamic Interface Number: %d\n",
		ndanm, ndanm->type,
		ndanm->if_num);
}

/*
 * nss_dynamic_interface_dealloc_node_msg()
 *	Log Dynamic Interface dealloc node message.
 */
static void nss_dynamic_interface_dealloc_node_log_msg(struct nss_dynamic_interface_msg *ndm)
{
	struct nss_dynamic_interface_dealloc_node_msg *nddnm __maybe_unused = &ndm->msg.dealloc_node;
	nss_trace("%px: NSS Dynamic Interface Alloc Node Message:\n"
		"Dynamic Interface Type: %d\n"
		"Dynamic Interface Number: %d\n",
		nddnm, nddnm->type,
		nddnm->if_num);
}

/*
 * nss_dynamic_interface_log_verbose()
 *	Log message contents.
 */
static void nss_dynamic_interface_log_verbose(struct nss_dynamic_interface_msg *ndm)
{
	switch (ndm->cm.type) {
	case NSS_DYNAMIC_INTERFACE_ALLOC_NODE:
		nss_dynamic_interface_alloc_node_log_msg(ndm);
		break;

	case NSS_DYNAMIC_INTERFACE_DEALLOC_NODE:
		nss_dynamic_interface_dealloc_node_log_msg(ndm);
		break;

	default:
		nss_warning("%px: Invalid message type\n", ndm);
		break;
	}
}

/*
 * nss_dynamic_interface_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_dynamic_interface_log_tx_msg(struct nss_dynamic_interface_msg *ndm)
{
	if (ndm->cm.type >= NSS_DYNAMIC_INTERFACE_MAX) {
		nss_warning("%px: Invalid message type\n", ndm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ndm, ndm->cm.type, nss_dynamic_interface_log_message_types_str[ndm->cm.type]);
	nss_dynamic_interface_log_verbose(ndm);
}

/*
 * nss_dynamic_interface_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_dynamic_interface_log_rx_msg(struct nss_dynamic_interface_msg *ndm)
{
	if (ndm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ndm);
		return;
	}

	if (ndm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ndm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ndm, ndm->cm.type,
			nss_dynamic_interface_log_message_types_str[ndm->cm.type],
			ndm->cm.response, nss_cmn_response_str[ndm->cm.response]);
		goto verbose;
	}

	if (ndm->cm.error >= NSS_DYNAMIC_INTERFACE_ERR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ndm, ndm->cm.type, nss_dynamic_interface_log_message_types_str[ndm->cm.type],
			ndm->cm.response, nss_cmn_response_str[ndm->cm.response],
			ndm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n, error[%d]:%s\n",
		ndm, ndm->cm.type, nss_dynamic_interface_log_message_types_str[ndm->cm.type],
		ndm->cm.response, nss_cmn_response_str[ndm->cm.response],
		ndm->cm.error, nss_dynamic_interface_log_error_response_types_str[ndm->cm.error]);

verbose:
	nss_dynamic_interface_log_verbose(ndm);
}
