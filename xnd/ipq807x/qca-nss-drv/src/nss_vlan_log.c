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
 * nss_vlan_log.c
 *	NSS VLAN logger file.
 */

#include "nss_core.h"

/*
 * nss_vlan_log_message_types_str
 *	VLAN message strings
 */
static int8_t *nss_vlan_log_message_types_str[NSS_VLAN_MSG_TYPE_MAX] __maybe_unused = {
	"VLAN ADD TAG",
};

/*
 * nss_vlan_log_error_response_types_str
 *	Strings for error types for VLAN messages
 */
static int8_t *nss_vlan_log_error_response_types_str[NSS_VLAN_ERROR_TYPE_MAX] __maybe_unused = {
	"VLAN Unknown Message",
};

/*
 * nss_vlan_log_add_tag_msg()
 *	Log NSS VLAN Add Tag message.
 */
static void nss_vlan_log_add_tag_msg(struct nss_vlan_msg *nvm)
{
	struct nss_vlan_msg_add_tag *nvtm __maybe_unused = &nvm->msg.add_tag;
	nss_trace("%px: NSS VLAN Add Tag Message:\n"
		"VLAN Tag: %d\n"
		"VLAN Next Hop: %d\n"
		"VLAN Interface Number: %d\n",
		nvtm, nvtm->vlan_tag,
		nvtm->next_hop, nvtm->if_num);
}

/*
 * nss_vlan_log_verbose()
 *	Log message contents.
 */
static void nss_vlan_log_verbose(struct nss_vlan_msg *nvm)
{
	switch (nvm->cm.type) {
	case NSS_VLAN_MSG_ADD_TAG:
		nss_vlan_log_add_tag_msg(nvm);
		break;

	default:
		nss_warning("%px: Invalid message type\n", nvm);
		break;
	}
}

/*
 * nss_vlan_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_vlan_log_tx_msg(struct nss_vlan_msg *nvm)
{
	if (nvm->cm.type >= NSS_VLAN_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", nvm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nvm, nvm->cm.type, nss_vlan_log_message_types_str[nvm->cm.type]);
	nss_vlan_log_verbose(nvm);
}

/*
 * nss_vlan_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_vlan_log_rx_msg(struct nss_vlan_msg *nvm)
{
	if (nvm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nvm);
		return;
	}

	if (nvm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nvm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nvm, nvm->cm.type,
			nss_vlan_log_message_types_str[nvm->cm.type],
			nvm->cm.response, nss_cmn_response_str[nvm->cm.response]);
		goto verbose;
	}

	if (nvm->cm.error >= NSS_VLAN_ERROR_TYPE_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nvm, nvm->cm.type, nss_vlan_log_message_types_str[nvm->cm.type],
			nvm->cm.response, nss_cmn_response_str[nvm->cm.response],
			nvm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nvm, nvm->cm.type, nss_vlan_log_message_types_str[nvm->cm.type],
		nvm->cm.response, nss_cmn_response_str[nvm->cm.response],
		nvm->cm.error, nss_vlan_log_error_response_types_str[nvm->cm.error]);

verbose:
	nss_vlan_log_verbose(nvm);
}
