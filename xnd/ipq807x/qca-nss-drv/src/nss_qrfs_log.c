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
 * nss_qrfs_log.c
 *	NSS QRFS logger file.
 */

#include "nss_core.h"
#define NSS_QRFS_LOG_IPV4 4
#define NSS_QRFS_LOG_IPV6 6

/*
 * nss_qrfs_log_message_types_str
 *	QRFS message strings
 */
static int8_t *nss_qrfs_log_message_types_str[NSS_QRFS_MSG_MAX] __maybe_unused = {
	"QRFS Flow Add Message",
	"QRFS Flow Delete Message",
	"QRFS MAC Add Message",
	"QRFS MAC Delete Message",
	"QRFS Stats Sync",
};

/*
 * nss_qrfs_log_error_response_types_str
 *	Strings for error types for QRFS messages
 */
static int8_t *nss_qrfs_log_error_response_types_str[NSS_QRFS_ERROR_MAX] __maybe_unused = {
	"QRFS Invalid Message Type",
	"QRFS Invalid Message Size",
	"QRFS Invalid IP Version",
	"QRFS V4 Flow Table Full",
	"QRFS V6 Flow Table Full",
	"QRFS MAC Table Full",
};

/*
 * nss_qrfs_log_flow_rule_msg()
 *	Log NSS QRFS Flow Rule Message.
 */
static void nss_qrfs_log_flow_rule_msg(struct nss_qrfs_flow_rule_msg *nqfm)
{
	nss_trace("%px: NSS QRFS Flow Rule Message:\n"
		"QRFS Source Port: %d\n"
		"QRFS Destination Port: %d\n"
		"QRFS IP Version: %d\n"
		"QRFS Protcol: %d\n"
		"QRFS CPU ID: %d\n"
		"QRFS Physical Interface Number: %d\n",
		nqfm, nqfm->src_port,
		nqfm->dst_port, nqfm->ip_version,
		nqfm->protocol, nqfm->cpu,
		nqfm->if_num);

	/*
	 * Continuation of log. Different identifiers based on ip_version
	 */
	if (nqfm->ip_version == NSS_QRFS_LOG_IPV6) {
		nss_trace("QRFS Source Address: %pI6\n"
			"QRFS Destination Address: %pI6\n",
			nqfm->src_addr, nqfm->dst_addr);
	} else if (nqfm->ip_version == NSS_QRFS_LOG_IPV4) {
		nss_trace("QRFS Source Address: %pI4\n"
			"QRFS Destination Address: %pI4\n",
			nqfm->src_addr, nqfm->dst_addr);
	}
}

/*
 * nss_qrfs_log_mac_rule_msg()
 *	Log NSS QRFS MAC Rule Message.
 */
static void nss_qrfs_log_mac_rule_msg(struct nss_qrfs_mac_rule_msg *nqmm)
{
	nss_trace("%px: NSS QRFS MAC Rule Message:\n"
		"QRFS MAC: %pM\n"
		"QRFS CPU ID: %d\n"
		"QRFS Physical Interface Number: %d\n",
		nqmm, nqmm->mac,
		nqmm->cpu, nqmm->if_num);
}

/*
 * nss_qrfs_log_verbose()
 *	Log message contents.
 */
static void nss_qrfs_log_verbose(struct nss_qrfs_msg *nqm)
{
	switch (nqm->cm.type) {
	case NSS_QRFS_MSG_FLOW_ADD:
	case NSS_QRFS_MSG_FLOW_DELETE:
		nss_qrfs_log_flow_rule_msg(&nqm->msg.flow_add);
		break;

	case NSS_QRFS_MSG_MAC_ADD:
	case NSS_QRFS_MSG_MAC_DELETE:
		nss_qrfs_log_mac_rule_msg(&nqm->msg.mac_add);
		break;

	case NSS_QRFS_MSG_STATS_SYNC:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_warning("%px: Invalid message type\n", nqm);
		break;
	}
}

/*
 * nss_qrfs_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_qrfs_log_tx_msg(struct nss_qrfs_msg *nqm)
{
	if (nqm->cm.type >= NSS_QRFS_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", nqm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nqm, nqm->cm.type, nss_qrfs_log_message_types_str[nqm->cm.type]);
	nss_qrfs_log_verbose(nqm);
}

/*
 * nss_qrfs_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_qrfs_log_rx_msg(struct nss_qrfs_msg *nqm)
{
	if (nqm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nqm);
		return;
	}

	if (nqm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nqm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nqm, nqm->cm.type,
			nss_qrfs_log_message_types_str[nqm->cm.type],
			nqm->cm.response, nss_cmn_response_str[nqm->cm.response]);
		goto verbose;
	}

	if (nqm->cm.error >= NSS_QRFS_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nqm, nqm->cm.type, nss_qrfs_log_message_types_str[nqm->cm.type],
			nqm->cm.response, nss_cmn_response_str[nqm->cm.response],
			nqm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nqm, nqm->cm.type, nss_qrfs_log_message_types_str[nqm->cm.type],
		nqm->cm.response, nss_cmn_response_str[nqm->cm.response],
		nqm->cm.error, nss_qrfs_log_error_response_types_str[nqm->cm.error]);

verbose:
	nss_qrfs_log_verbose(nqm);
}
