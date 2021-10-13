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
 * nss_ipsec_log.c
 *	NSS IPSEC logger file.
 */

#include "nss_core.h"

#define NSS_IPSEC_LOG_IPV4 4
#define NSS_IPSEC_LOG_IPV6 6

/*
 * nss_ipsec_log_message_types_str
 *	IPSEC message strings
 */
static int8_t *nss_ipsec_log_message_types_str[NSS_IPSEC_MSG_TYPE_MAX] __maybe_unused = {
	"IPSEC Msg None",
	"IPSEC ADD Rule",
	"IPSEC DEL Rule",
	"IPSEC Flush Tunnel",
	"IPSEC SA Stats",
	"IPSEC Flow Stats",
	"IPSEC Node Stats",
	"IPSEC Configure Node",
};

/*
 * nss_ipsec_log_error_response_types_str
 *	Strings for error types for IPSEC messages
 */
static int8_t *nss_ipsec_log_error_response_types_str[NSS_IPSEC_ERROR_TYPE_MAX] __maybe_unused = {
	"IPSEC No Error",
	"IPSEC Hash Duplicate",
	"IPSEC Hash Collision",
	"IPSEC Unhandled Message",
	"IPSEC Invalid Rule",
	"IPSEC MAX SA",
	"IPSEC MAX Flow",
	"IPSEC Invalid CINDEX",
	"IPSEC Invalid IP Version",
};

/*
 * nss_ipsec_log_rule_msg()
 *	Log NSS IPSEC rule message.
 */
static void nss_ipsec_log_rule_msg(struct nss_ipsec_msg *nim)
{
	struct nss_ipsec_rule *nir __maybe_unused = &nim->msg.rule;

	nss_trace("%px: NSS IPSEC Rule Message:\n"
		"IPSEC ESP SPI Index: %dn"
		"IPSEC TTL Hop Limit: %dn"
		"IPSEC IP Version: %x\n"
		"IPSEC Crypto Index: %d\n"
		"IPSEC Window Size: %d\n"
		"IPSEC Cipher Block Len: %d\n"
		"IPSEC Initialization Vector Length: %d\n"
		"IPSEC NAT-T Required: %d\n"
		"IPSEC ICV Length: %d\n"
		"IPSEC Skip Seq Number: %d\n"
		"IPSEC Skip ESP Trailer: %d\n"
		"IPSEC Use Pattern: %d\n"
		"IPSEC Enable Extended Sequence Number: %d\n"
		"IPSEC DSCP Value: %d\n"
		"IPSEC Don't Fragment Flag: %d\n"
		"IPSEC DSCP Copy %d\n"
		"IPSEC DF Copy: %d\n"
		"IPSEC NSS Index: %d\n"
		"IPSEC SA Index: %d\n",
		nir, nir->oip.esp_spi,
		nir->oip.ttl_hop_limit, nir->oip.ip_ver,
		nir->data.crypto_index, nir->data.window_size,
		nir->data.cipher_blk_len, nir->data.iv_len,
		nir->data.nat_t_req, nir->data.esp_icv_len,
		nir->data.esp_seq_skip, nir->data.esp_tail_skip,
		nir->data.use_pattern, nir->data.enable_esn,
		nir->data.dscp, nir->data.df,
		nir->data.copy_dscp, nir->data.copy_df,
		nir->index, nir->sa_idx);

	/*
	 * Continuation of previous log. Different identifiers based on ip_ver
	 */
	if (nir->oip.ip_ver == NSS_IPSEC_LOG_IPV6) {
		nss_trace("IPSEC Destination Address: %pI6\n"
			"IPSEC Source Address: %pI6\n",
			nir->oip.dst_addr, nir->oip.src_addr);
	} else if (nir->oip.ip_ver == NSS_IPSEC_LOG_IPV4) {
		nss_trace("IPSEC Destination Address: %pI4\n"
			"IPSEC Source Address: %pI4\n",
			nir->oip.dst_addr, nir->oip.src_addr);
	}
}

/*
 * nss_ipsec_log_configure_node_msg()
 *	Log NSS IPSEC configure node message.
 */
static void nss_ipsec_log_configure_node_msg(struct nss_ipsec_msg *nim)
{
	struct nss_ipsec_configure_node *nicn __maybe_unused = &nim->msg.node;
	nss_trace("%px: NSS IPSEC Configure Node\n"
		"IPSEC DMA Redirect: %d\n"
		"IPSEC DMA Lookaside: %d\n",
		nicn, nicn->dma_redirect,
		nicn->dma_lookaside);
}

/*
 * nss_ipsec_log_verbose()
 *	Log message contents.
 */
static void nss_ipsec_log_verbose(struct nss_ipsec_msg *nim)
{
	switch (nim->cm.type) {
	case NSS_IPSEC_MSG_TYPE_ADD_RULE:
	case NSS_IPSEC_MSG_TYPE_DEL_RULE:
		nss_ipsec_log_rule_msg(nim);
		break;

	case NSS_IPSEC_MSG_TYPE_CONFIGURE_NODE:
		nss_ipsec_log_configure_node_msg(nim);
		break;

	case NSS_IPSEC_MSG_TYPE_NONE:
	case NSS_IPSEC_MSG_TYPE_FLUSH_TUN:
	case NSS_IPSEC_MSG_TYPE_SYNC_SA_STATS:
	case NSS_IPSEC_MSG_TYPE_SYNC_FLOW_STATS:
	case NSS_IPSEC_MSG_TYPE_SYNC_NODE_STATS:
		/*
		 * No log for these valid messages.
		 */
		break;

	default:
		nss_warning("%px: Invalid message type\n", nim);
		break;
	}
}

/*
 * nss_ipsec_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_ipsec_log_tx_msg(struct nss_ipsec_msg *nim)
{
	if (nim->cm.type >= NSS_IPSEC_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", nim);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nim, nim->cm.type, nss_ipsec_log_message_types_str[nim->cm.type]);
	nss_ipsec_log_verbose(nim);
}

/*
 * nss_ipsec_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_ipsec_log_rx_msg(struct nss_ipsec_msg *nim)
{
	if (nim->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nim);
		return;
	}

	if (nim->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nim->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nim, nim->cm.type,
			nss_ipsec_log_message_types_str[nim->cm.type],
			nim->cm.response, nss_cmn_response_str[nim->cm.response]);
		goto verbose;
	}

	if (nim->cm.error >= NSS_IPSEC_ERROR_TYPE_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nim, nim->cm.type, nss_ipsec_log_message_types_str[nim->cm.type],
			nim->cm.response, nss_cmn_response_str[nim->cm.response],
			nim->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nim, nim->cm.type, nss_ipsec_log_message_types_str[nim->cm.type],
		nim->cm.response, nss_cmn_response_str[nim->cm.response],
		nim->cm.error, nss_ipsec_log_error_response_types_str[nim->cm.error]);

verbose:
	nss_ipsec_log_verbose(nim);
}
