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
 * nss_gre_redir_lag_us_log.c
 *	NSS GRE REDIR LAG US logger file.
 */

#include "nss_core.h"

/*
 * nss_gre_redir_lag_us_log_message_types_str
 *	GRE REDIR LAG US message strings
 */
static int8_t *nss_gre_redir_lag_us_log_message_types_str[NSS_GRE_REDIR_LAG_US_MAX_MSG_TYPES] __maybe_unused = {
	"GRE REDIR LAG US config Message",
	"GRE REDIR LAG US add hash node message",
	"GRE REDIR LAG US delete hash node message",
	"GRE REDIR LAG US query hash node message",
	"GRE REDIR LAG US stats sync message",
	"GRE REDIR LAG US DB hash node message",
};

/*
 * nss_gre_redir_lag_us_log_error_response_types_str
 *	Strings for error types for GRE REDIR LAG US messages
 */
static int8_t *nss_gre_redir_lag_us_log_error_response_types_str[NSS_GRE_REDIR_LAG_ERR_MAX] __maybe_unused = {
	"GRE REDIR LAG Success",
	"GRE REDIR LAG Incorrect Interface",
	"GRE REDIR LAG US Core Unregister Failed",
	"GRE REDIR LAG US STats Index Not Found",
	"GRE REDIR LAG Dealloc Failed",
};

/*
 * nss_gre_redir_lag_us_log_config_msg()
 *	Log NSS GRE REDIR LAG US config message.
 */
static void nss_gre_redir_lag_us_log_config_msg(struct nss_gre_redir_lag_us_msg *ngm)
{
	struct nss_gre_redir_lag_us_config_msg *ngcm __maybe_unused = &ngm->msg.config_us;
	nss_trace("%px: NSS GRE REDIR LAG Config Message:\n"
		"GRE REDIR LAG US Hash Mode: %d\n"
		"GRE REDIR LAG US Number of Slaves: %d\n"
		"GRE REDIR LAG US Interface Number: %px\n",
		ngcm, ngcm->hash_mode, ngcm->num_slaves,
		ngcm->if_num);
}

/*
 * nss_gre_redir_lag_us_log_add_hash_node_msg()
 *	Log NSS GRE REDIR LAG US add hash node message.
 */
static void nss_gre_redir_lag_us_log_add_hash_node_msg(struct nss_gre_redir_lag_us_msg *ngm)
{
	struct nss_gre_redir_lag_us_add_hash_node_msg *ngam __maybe_unused = &ngm->msg.add_hash;
	nss_trace("%px: NSS GRE REDIR LAG Add Hash Node Message:\n"
		"GRE REDIR LAG US Interface Number: %d\n"
		"GRE REDIR LAG US Source MAC: %px\n"
		"GRE REDIR LAG US Destination MAC: %px\n",
		ngam, ngam->if_num, ngam->src_mac,
		ngam->dest_mac);
}

/*
 * nss_gre_redir_lag_us_log_del_hash_node_msg()
 *	Log NSS GRE REDIR LAG US del hash node message.
 */
static void nss_gre_redir_lag_us_log_del_hash_node_msg(struct nss_gre_redir_lag_us_msg *ngm)
{
	struct nss_gre_redir_lag_us_del_hash_node_msg *ngdm __maybe_unused = &ngm->msg.del_hash;
	nss_trace("%px: NSS GRE REDIR LAG Del Hash Node Message:\n"
		"GRE REDIR LAG US Source MAC: %px\n"
		"GRE REDIR LAG US Destination MAC: %px\n",
		ngdm, ngdm->src_mac,ngdm->dest_mac);
}

/*
 * nss_gre_redir_lag_us_log_query_hash_node_msg()
 *	Log NSS GRE REDIR LAG US query hash node message.
 */
static void nss_gre_redir_lag_us_log_query_hash_node_msg(struct nss_gre_redir_lag_us_msg *ngm)
{
	struct nss_gre_redir_lag_us_query_hash_node_msg *ngqm __maybe_unused = &ngm->msg.query_hash;
	nss_trace("%px: NSS GRE REDIR LAG Query Hash Node Message:\n"
		"GRE REDIR LAG US Source MAC: %px\n"
		"GRE REDIR LAG US Destination MAC: %px\n"
		"GRE REDIR LAG US Interface Number: %d\n",
		ngqm, ngqm->src_mac, ngqm->dest_mac,
		ngqm->ifnum);
}

/*
 * nss_gre_redir_lag_us_log_verbose()
 *	Log message contents.
 */
static void nss_gre_redir_lag_us_log_verbose(struct nss_gre_redir_lag_us_msg *ngm)
{
	switch (ngm->cm.type) {
	case NSS_GRE_REDIR_LAG_US_CONFIG_MSG:
		nss_gre_redir_lag_us_log_config_msg(ngm);
		break;

	case NSS_GRE_REDIR_LAG_US_ADD_HASH_NODE_MSG:
		nss_gre_redir_lag_us_log_add_hash_node_msg(ngm);
		break;

	case NSS_GRE_REDIR_LAG_US_DEL_HASH_NODE_MSG:
		nss_gre_redir_lag_us_log_del_hash_node_msg(ngm);
		break;

	case NSS_GRE_REDIR_LAG_US_QUERY_HASH_NODE_MSG:
		nss_gre_redir_lag_us_log_query_hash_node_msg(ngm);
		break;

	case NSS_GRE_REDIR_LAG_US_CMN_STATS_SYNC_MSG:
	case NSS_GRE_REDIR_LAG_US_DB_HASH_NODE_MSG:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_warning("%px: Invalid message type\n", ngm);
		break;
	}
}

/*
 * nss_gre_redir_lag_us_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_gre_redir_lag_us_log_tx_msg(struct nss_gre_redir_lag_us_msg *ngm)
{
	if (ngm->cm.type >= NSS_GRE_REDIR_LAG_US_MAX_MSG_TYPES) {
		nss_warning("%px: Invalid message type\n", ngm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ngm, ngm->cm.type, nss_gre_redir_lag_us_log_message_types_str[ngm->cm.type]);
	nss_gre_redir_lag_us_log_verbose(ngm);
}

/*
 * nss_gre_redir_lag_us_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_gre_redir_lag_us_log_rx_msg(struct nss_gre_redir_lag_us_msg *ngm)
{
	if (ngm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ngm);
		return;
	}

	if (ngm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ngm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ngm, ngm->cm.type,
			nss_gre_redir_lag_us_log_message_types_str[ngm->cm.type],
			ngm->cm.response, nss_cmn_response_str[ngm->cm.response]);
		goto verbose;
	}

	if (ngm->cm.error >= NSS_GRE_REDIR_LAG_ERR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ngm, ngm->cm.type, nss_gre_redir_lag_us_log_message_types_str[ngm->cm.type],
			ngm->cm.response, nss_cmn_response_str[ngm->cm.response],
			ngm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ngm, ngm->cm.type, nss_gre_redir_lag_us_log_message_types_str[ngm->cm.type],
		ngm->cm.response, nss_cmn_response_str[ngm->cm.response],
		ngm->cm.error, nss_gre_redir_lag_us_log_error_response_types_str[ngm->cm.error]);

verbose:
	nss_gre_redir_lag_us_log_verbose(ngm);
}
