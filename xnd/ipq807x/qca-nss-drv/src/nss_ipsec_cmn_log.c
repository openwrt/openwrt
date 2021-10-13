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
 * nss_ipsec_cmn_log.c
 *	NSS IPSEC logger file.
 */

#include "nss_core.h"

#define NSS_IPSEC_LOG_IPV4 4
#define NSS_IPSEC_LOG_IPV6 6

/*
 * nss_ipsec_cmn_log_msg_types_str
 *	IPSEC message strings
 */
static int8_t *nss_ipsec_cmn_log_msg_types_str[NSS_IPSEC_CMN_MSG_TYPE_MAX] __maybe_unused = {
	"IPSEC CMN Msg None",
	"IPSEC CMN Node Config",
	"IPSEC CMN CTX Config",
	"IPSEC CMN CTX Sync",
	"IPSEC CMN SA Create",
	"IPSEC CMN SA Destroy",
	"IPSEC CMN SA Sync",
	"IPSEC CMN Flow Create",
	"IPSEC CMN Flow Destroy",
};

/*
 * nss_ipsec_cmn_log_node_msg_types_str
 *	IPSEC cmn node message strings
 */
static int8_t *nss_ipsec_cmn_log_node_str[] __maybe_unused = {
	"IPSEC CMN Node DMA Redirect",
	"IPSEC CMN Node DMA Lookaside",
	"IPSEC CMN Node Maximum SA",
};

/*
 * nss_ipsec_cmn_log_ctx_msg_types_str
 *	IPSEC cmn ctx message strings
 */
static int8_t *nss_ipsec_cmn_log_ctx_str[] __maybe_unused = {
	"IPSEC CMN CTX Type",
	"IPSEC CMN CTX Exception Interface",
};

/*
 * nss_ipsec_cmn_log_ctx_types_str
 *	IPSEC cmn context strings
 */
static int8_t *nss_ipsec_cmn_ctx_types_str[] __maybe_unused = {
	"IPSEC CMN CTX NONE",
	"IPSEC CMN CTX INNER",
	"IPSEC CMN CTX INNER BOUNCE",
	"IPSEC CMN CTX OUTER",
	"IPSEC CMN CTX OUTER BOUNCE",
	"IPSEC CMN CTX REDIRECT",
};

/*
 * nss_ipsec_cmn_log_flow_tuple_str
 *	IPSEC cmn flow tuple strings
 */
static int8_t *nss_ipsec_cmn_log_flow_tuple_str[] __maybe_unused = {
	"Dest IP",
	"Src IP",
	"Spi Index",
	"Dest Port",
	"Src Port",
	"User Pattern",
	"User Protocol",
	"IP Version",
};

/*
 * nss_ipsec_cmn_log_sa_tuple_str
 *	IPSEC cmn SA tuple strings
 */
static int8_t *nss_ipsec_cmn_log_sa_tuple_str[] __maybe_unused = {
	"Dest IP",
	"Src IP",
	"Spi Index",
	"Dest Port",
	"Src Port",
	"Crypto Index",
	"Protocol",
	"IP Version",
	"Hop Limit",
};

/*
 * nss_ipsec_cmn_log_sa_data_str
 *	IPSEC cmn SA tuple strings
 */
static int8_t *nss_ipsec_cmn_log_sa_data_str[] __maybe_unused = {
	"Sequence Start",
	"Flags",
	"Window Size",
	"DSCP",
	"DF",
	"Block Length",
	"IV length",
	"ICV length",
};

/*
 * nss_ipsec_cmn_log_error_str
 *	Strings for error types for IPSEC messages
 */
static int8_t *nss_ipsec_cmn_log_error_str[NSS_IPSEC_CMN_MSG_ERROR_MAX] __maybe_unused = {
	"IPSEC No Error",
	"IPSEC Invalid Context",
	"IPSEC SA allocation Error",
	"IPSEC Invalid SA",
	"IPSEC Duplicate SA",
	"IPSEC SA is in Use",
	"IPSEC Error in Flow Allocation",
	"IPSEC Invalid Flow",
	"IPSEC Duplicate Flow",
	"IPSEC Failure to find SA for Flow",
	"IPSEC Failed to Register Dynamic Interface",
	"IPSEC Unhandled Message",
};

/*
 * nss_ipsec_cmn_log_node_msg()
 *	Log NSS IPSEC node message.
 */
static void nss_ipsec_cmn_log_node_msg(struct nss_ipsec_cmn_msg *nim)
{
	struct nss_ipsec_cmn_node *node_msg __maybe_unused = &nim->msg.node;

	nss_trace("%px: NSS IPSEC Node Message:\n"
		  "%s: %d\n"
		  "%s: %d\n"
		  "%s: %d\n", nim,
		  nss_ipsec_cmn_log_node_str[0], node_msg->dma_redirect,
		  nss_ipsec_cmn_log_node_str[1], node_msg->dma_lookaside,
		  nss_ipsec_cmn_log_node_str[2], node_msg->max_sa);
}

/*
 * nss_ipsec_cmn_log_ctx_msg()
 *	Log NSS IPSEC ctx message.
 */
static void nss_ipsec_cmn_log_ctx_msg(struct nss_ipsec_cmn_msg *nim)
{
	struct nss_ipsec_cmn_ctx *ctx_msg __maybe_unused = &nim->msg.ctx;

	nss_trace("%px: NSS IPSEC CTX Message:\n"
		  "%s: %s\n"
		  "%s: %d\n", nim,
		  nss_ipsec_cmn_log_ctx_str[0], nss_ipsec_cmn_ctx_types_str[ctx_msg->type],
		  nss_ipsec_cmn_log_ctx_str[1], ctx_msg->except_ifnum);
}

/*
 * nss_ipsec_cmn_log_sa_msg()
 *	Log NSS IPSEC SA message.
 */
static void nss_ipsec_cmn_log_sa_msg(struct nss_ipsec_cmn_msg *nim)
{
	struct nss_ipsec_cmn_sa *sa_msg __maybe_unused = &nim->msg.sa;
	struct nss_ipsec_cmn_sa_tuple *tuple = &sa_msg->sa_tuple;
	struct nss_ipsec_cmn_sa_data *data __maybe_unused = &sa_msg->sa_data;

	nss_trace("%px: NSS IPSEC SA Message:\n", nim);

	if (tuple->ip_ver == 4) {
		nss_trace("%s: %pI4\n%s: %pI4\n",
			  nss_ipsec_cmn_log_sa_tuple_str[0], tuple->dest_ip,
			  nss_ipsec_cmn_log_sa_tuple_str[1], tuple->src_ip);
	} else {
		nss_trace("%s: %pI6\n%s: %pI6\n",
			  nss_ipsec_cmn_log_sa_tuple_str[0], tuple->dest_ip,
			  nss_ipsec_cmn_log_sa_tuple_str[1], tuple->src_ip);
	}

	nss_trace( "%s: %x\n%s: %d\n%s: %d\n%s: %d\n"
		  "%s: %d\n%s: %d\n%s: %d\n"
		  "%s: %d\n%s: %x\n%s: %d\n%s: %d\n"
		  "%s: %d\n%s: %d\n%s: %d\n%s: %d\n",
		  nss_ipsec_cmn_log_sa_tuple_str[2], tuple->spi_index,
		  nss_ipsec_cmn_log_sa_tuple_str[3], tuple->dest_port,
		  nss_ipsec_cmn_log_sa_tuple_str[4], tuple->src_port,
		  nss_ipsec_cmn_log_sa_tuple_str[5], tuple->crypto_index,
		  nss_ipsec_cmn_log_sa_tuple_str[6], tuple->protocol,
		  nss_ipsec_cmn_log_sa_tuple_str[7], tuple->ip_ver,
		  nss_ipsec_cmn_log_sa_tuple_str[8], tuple->hop_limit,

		  nss_ipsec_cmn_log_sa_data_str[0], data->seq_start,
		  nss_ipsec_cmn_log_sa_data_str[1], data->flags,
		  nss_ipsec_cmn_log_sa_data_str[2], data->window_size,
		  nss_ipsec_cmn_log_sa_data_str[3], data->dscp,
		  nss_ipsec_cmn_log_sa_data_str[4], data->df,
		  nss_ipsec_cmn_log_sa_data_str[5], data->blk_len,
		  nss_ipsec_cmn_log_sa_data_str[6], data->iv_len,
		  nss_ipsec_cmn_log_sa_data_str[7], data->icv_len);

}

/*
 * nss_ipsec_cmn_log_flow_msg()
 *	Log NSS IPSEC Flow message.
 */
static void nss_ipsec_cmn_log_flow_msg(struct nss_ipsec_cmn_msg *nim)
{
	struct nss_ipsec_cmn_flow *flow_msg __maybe_unused = &nim->msg.flow;
	struct nss_ipsec_cmn_flow_tuple *flow = &flow_msg->flow_tuple;
	struct nss_ipsec_cmn_sa_tuple *sa = &flow_msg->sa_tuple;

	nss_trace("%px: NSS IPSEC Flow Message:\n", nim);

	if (sa->ip_ver == 4) {
		nss_trace("%s: %pI4\n%s: %pI4\n",
			  nss_ipsec_cmn_log_sa_tuple_str[0], sa->dest_ip,
			  nss_ipsec_cmn_log_sa_tuple_str[1], sa->src_ip);
	} else {
		nss_trace("%s: %pI6\n%s: %pI6\n",
			  nss_ipsec_cmn_log_sa_tuple_str[0], sa->dest_ip,
			  nss_ipsec_cmn_log_sa_tuple_str[1], sa->src_ip);
	}

	if (flow->ip_ver == 4) {
		nss_trace("%s: %pI4\n%s: %pI4\n",
			  nss_ipsec_cmn_log_sa_tuple_str[0], flow->dest_ip,
			  nss_ipsec_cmn_log_sa_tuple_str[1], flow->src_ip);
	} else {
		nss_trace("%s: %pI6\n%s: %pI6\n",
			  nss_ipsec_cmn_log_sa_tuple_str[0], flow->dest_ip,
			  nss_ipsec_cmn_log_sa_tuple_str[1], flow->src_ip);
	}

	nss_trace( "%s: %x\n%s: %d\n%s: %d\n%s: %d\n"
		  "%s: %d\n",
		  nss_ipsec_cmn_log_flow_tuple_str[2], flow->spi_index,
		  nss_ipsec_cmn_log_flow_tuple_str[3], flow->dst_port,
		  nss_ipsec_cmn_log_flow_tuple_str[4], flow->src_port,
		  nss_ipsec_cmn_log_flow_tuple_str[5], flow->user_pattern,
		  nss_ipsec_cmn_log_flow_tuple_str[6], flow->protocol);

	nss_trace( "%s: %x\n%s: %d\n%s: %d\n%s: %d\n"
		  "%s: %d\n%s: %d\n%s: %d\n",
		  nss_ipsec_cmn_log_sa_tuple_str[2], sa->spi_index,
		  nss_ipsec_cmn_log_sa_tuple_str[3], sa->dest_port,
		  nss_ipsec_cmn_log_sa_tuple_str[4], sa->src_port,
		  nss_ipsec_cmn_log_sa_tuple_str[5], sa->crypto_index,
		  nss_ipsec_cmn_log_sa_tuple_str[6], sa->protocol,
		  nss_ipsec_cmn_log_sa_tuple_str[7], sa->ip_ver,
		  nss_ipsec_cmn_log_sa_tuple_str[8], sa->hop_limit);
}

/*
 * nss_ipsec_cmn_log_verbose()
 *	Log message contents.
 */
static void nss_ipsec_cmn_log_verbose(struct nss_ipsec_cmn_msg *nim)
{
	switch (nim->cm.type) {
	case NSS_IPSEC_CMN_MSG_TYPE_NODE_CONFIG:
		nss_ipsec_cmn_log_node_msg(nim);
		break;

	case NSS_IPSEC_CMN_MSG_TYPE_CTX_CONFIG:
		nss_ipsec_cmn_log_ctx_msg(nim);
		break;

	case NSS_IPSEC_CMN_MSG_TYPE_SA_CREATE:
	case NSS_IPSEC_CMN_MSG_TYPE_SA_DESTROY:
		nss_ipsec_cmn_log_sa_msg(nim);
		break;

	case NSS_IPSEC_CMN_MSG_TYPE_FLOW_CREATE:
	case NSS_IPSEC_CMN_MSG_TYPE_FLOW_DESTROY:
		nss_ipsec_cmn_log_flow_msg(nim);
		break;

	case NSS_IPSEC_CMN_MSG_TYPE_CTX_SYNC:
	case NSS_IPSEC_CMN_MSG_TYPE_SA_SYNC:
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
 * nss_ipsec_cmn_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_ipsec_cmn_log_tx_msg(struct nss_ipsec_cmn_msg *nim)
{
	if (nim->cm.type >= NSS_IPSEC_CMN_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", nim);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nim, nim->cm.type, nss_ipsec_cmn_log_msg_types_str[nim->cm.type]);
	nss_ipsec_cmn_log_verbose(nim);
}

/*
 * nss_ipsec_cmn_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_ipsec_cmn_log_rx_msg(struct nss_ipsec_cmn_msg *nim)
{
	if (nim->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nim);
		return;
	}

	if (nim->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nim->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nim, nim->cm.type,
			nss_ipsec_cmn_log_msg_types_str[nim->cm.type],
			nim->cm.response, nss_cmn_response_str[nim->cm.response]);
		goto verbose;
	}

	if (nim->cm.error >= NSS_IPSEC_CMN_MSG_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nim, nim->cm.type, nss_ipsec_cmn_log_msg_types_str[nim->cm.type],
			nim->cm.response, nss_cmn_response_str[nim->cm.response],
			nim->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nim, nim->cm.type, nss_ipsec_cmn_log_msg_types_str[nim->cm.type],
		nim->cm.response, nss_cmn_response_str[nim->cm.response],
		nim->cm.error, nss_ipsec_cmn_log_error_str[nim->cm.error]);

verbose:
	nss_ipsec_cmn_log_verbose(nim);
}
