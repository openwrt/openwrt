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
 * nss_gre_log.c
 *	NSS GRE logger file.
 */

#include "nss_core.h"

#define NSS_GRE_LOG_MESSAGE_TYPE_INDEX(type) ((type) - NSS_IF_MAX_MSG_TYPES - 1)

/*
 * nss_gre_log_message_types_str
 *	NSS GRE message strings
 */
static int8_t *nss_gre_log_message_types_str[NSS_GRE_MSG_MAX] __maybe_unused = {
	"GRE Message Configure",
	"GRE Message Deconfigure",
	"GRE Session Stats",
	"GRE Base Stats"
};

/*
 * nss_gre_log_config_msg()
 *	Log NSS GRE Config message.
 */
static void nss_gre_log_config_msg(struct nss_gre_msg *ngm)
{
	struct nss_gre_config_msg *ngcm __maybe_unused = &ngm->msg.cmsg;
	nss_trace("%px: NSS GRE Config message\n"
		"GRE flags: %d\n"
		"GRE ikey: %d\n"
		"GRE okey: %d\n"
		"GRE mode: %d\n"
		"GRE ip type: %d\n"
		"GRE interface number: %d\n"
		"GRE Src MAC: %pM\n"
		"GRE Dst MAC: %pM\n"
		"GRE ttl: %d\n"
		"GRE tos: %d\n"
		"GRE metadata size: %d\n",
		ngcm, ngcm->flags, ngcm->ikey, ngcm->okey,
		ngcm->mode, ngcm->ip_type, ngcm->next_node_if_num,
		ngcm->src_mac, ngcm->dest_mac, ngcm->ttl, ngcm->tos,
		ngcm->metadata_size);
	/*
	 * Continuation of the log message. Different identifiers based on IP type.
	 */
	if (ngcm->ip_type == NSS_GRE_IP_IPV6) {
		nss_trace("GRE Source IP: %pI6\n"
			"GRE Dest IP: %pI6\n",
			ngcm->src_ip, ngcm->dest_ip);
	} else {
		nss_trace("GRE Source IP: %pI4\n"
			"GRE Dest IP: %pI4\n",
			ngcm->src_ip, ngcm->dest_ip);
	}
}

/*
 * nss_gre_log_deconfig_msg()
 *	Log NSS GRE deconfig message.
 */
static void nss_gre_log_deconfig_msg(struct nss_gre_msg *ngm)
{
	struct nss_gre_deconfig_msg *ngdm __maybe_unused = &ngm->msg.dmsg;
	nss_trace("%px: NSS GRE deconfig message\n"
		"GRE interface number: %d\n",
		ngdm, ngdm->if_number);
}

/*
 * nss_gre_log_linkup_msg()
 *	Log NSS GRE linkup message.
 */
static void nss_gre_log_linkup_msg(struct nss_gre_msg *ngm)
{
	struct nss_gre_linkup_msg *nglm __maybe_unused = &ngm->msg.linkup;
	nss_trace("%px: NSS GRE linkup message\n"
		"GRE interface number: %d\n",
		nglm, nglm->if_number);
}

/*
 * nss_gre_log_linkdown_msg()
 *	Log NSS GRE linkdown message.
 */
static void nss_gre_log_linkdown_msg(struct nss_gre_msg *ngm)
{
	struct nss_gre_linkdown_msg *ngdm __maybe_unused = &ngm->msg.linkdown;
	nss_trace("%px: NSS GRE linkdown message\n"
		"GRE interface number: %d\n",
		ngdm, ngdm->if_number);
}

/*
 * nss_gre_log_verbose()
 *	Log message contents.
 */
static void nss_gre_log_verbose(struct nss_gre_msg *ngm)
{
	switch (ngm->cm.type) {
	case NSS_GRE_MSG_ENCAP_CONFIGURE:
	case NSS_GRE_MSG_DECAP_CONFIGURE:
		nss_gre_log_config_msg(ngm);
		break;

	case NSS_GRE_MSG_ENCAP_DECONFIGURE:
	case NSS_GRE_MSG_DECAP_DECONFIGURE:
		nss_gre_log_deconfig_msg(ngm);
		break;

	case NSS_IF_OPEN:
		nss_gre_log_linkup_msg(ngm);
		break;

	case NSS_IF_CLOSE:
		nss_gre_log_linkdown_msg(ngm);
		break;

	case NSS_GRE_MSG_SESSION_STATS:
	case NSS_GRE_MSG_BASE_STATS:
		/*
		 * No log for valid stats messages.
		 */
		break;

	default:
		nss_trace("%px: Invalid message type\n", ngm);
		break;
	}
}

/*
 * nss_gre_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_gre_log_tx_msg(struct nss_gre_msg *ngm)
{
	if (ngm->cm.type >= NSS_GRE_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", ngm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ngm, ngm->cm.type, nss_gre_log_message_types_str[NSS_GRE_LOG_MESSAGE_TYPE_INDEX(ngm->cm.type)]);
	nss_gre_log_verbose(ngm);
}

/*
 * nss_gre_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_gre_log_rx_msg(struct nss_gre_msg *ngm)
{
	if (ngm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ngm);
		return;
	}

	if (ngm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ngm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ngm, ngm->cm.type,
			nss_gre_log_message_types_str[NSS_GRE_LOG_MESSAGE_TYPE_INDEX(ngm->cm.type)],
			ngm->cm.response, nss_cmn_response_str[ngm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		ngm, ngm->cm.type, nss_gre_log_message_types_str[NSS_GRE_LOG_MESSAGE_TYPE_INDEX(ngm->cm.type)],
		ngm->cm.response, nss_cmn_response_str[ngm->cm.response]);

verbose:
	nss_gre_log_verbose(ngm);
}
