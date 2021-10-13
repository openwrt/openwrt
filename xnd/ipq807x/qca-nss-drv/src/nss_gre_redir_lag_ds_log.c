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
 * nss_gre_redir_lag_ds_log.c
 *	NSS GRE REDIR LAG DS logger file.
 */

#include "nss_core.h"

/*
 * nss_gre_redir_lag_ds_log_message_types_str
 *	GRE REDIR LAG DS message strings
 */
static int8_t *nss_gre_redir_lag_ds_log_message_types_str[NSS_GRE_REDIR_LAG_DS_MAX_MSG_TYPES] __maybe_unused = {
	"GRE REDIR LAG DS add station Message",
	"GRE REDIR LAG DS delete station message",
	"GRE REDIR LAG DS update station message",
	"GRE REDIR LAG DS stats sync message",
};

/*
 * nss_gre_redir_lag_ds_log_error_response_types_str
 *	Strings for error types for GRE REDIR LAG DS messages
 */
static int8_t *nss_gre_redir_lag_ds_log_error_response_types_str[NSS_GRE_REDIR_LAG_ERR_MAX] __maybe_unused = {
	"GRE REDIR LAG Success",
	"GRE REDIR LAG Incorrect Interface",
	"GRE REDIR LAG DS Core Unregister Failed",
	"GRE REDIR LAG DS STats Index Not Found",
	"GRE REDIR LAG Dealloc Failed",
};

/*
 * nss_gre_redir_lag_ds_log_add_sta_msg()
 *	Log NSS GRE REDIR LAG DS add STA message.
 */
static void nss_gre_redir_lag_ds_log_add_sta_msg(struct nss_gre_redir_lag_ds_msg *ngm)
{
	struct nss_gre_redir_lag_ds_add_sta_msg *ngasm __maybe_unused = &ngm->msg.add_sta;
	nss_trace("%px: NSS GRE REDIR LAG DS Add STA Message:\n"
		"GRE REDIR LAG DS Station MAC Address: %px\n"
		"GRE REDIR LAG DS Reorder Type: %d\n",
		ngasm, ngasm->mac, ngasm->reorder_type);
}

/*
 * nss_gre_redir_lag_ds_log_del_sta_msg()
 *	Log NSS GRE REDIR LAG DS del STA message.
 */
static void nss_gre_redir_lag_ds_log_del_sta_msg(struct nss_gre_redir_lag_ds_msg *ngm)
{
	struct nss_gre_redir_lag_ds_delete_sta_msg *ngdsm __maybe_unused = &ngm->msg.del_sta;
	nss_trace("%px: NSS GRE REDIR LAG DS Del STA Message:\n"
		"GRE REDIR LAG DS Station MAC Address: %px\n",
		ngdsm, ngdsm->mac);
}

/*
 * nss_gre_redir_lag_ds_log_add_sta_msg()
 *	Log NSS GRE REDIR LAG DS add STA message.
 */
static void nss_gre_redir_lag_ds_log_update_sta_msg(struct nss_gre_redir_lag_ds_msg *ngm)
{
	struct nss_gre_redir_lag_ds_update_sta_msg *ngusm __maybe_unused = &ngm->msg.update_sta;
	nss_trace("%px: NSS GRE REDIR LAG DS Update STA Message:\n"
		"GRE REDIR LAG DS Station MAC Address: %px\n"
		"GRE REDIR LAG DS Reorder Type: %d\n",
		ngusm, ngusm->mac, ngusm->reorder_type);
}

/*
 * nss_gre_redir_lag_ds_log_verbose()
 *	Log message contents.
 */
static void nss_gre_redir_lag_ds_log_verbose(struct nss_gre_redir_lag_ds_msg *ngm)
{
	switch (ngm->cm.type) {
	case NSS_GRE_REDIR_LAG_DS_ADD_STA_MSG:
		nss_gre_redir_lag_ds_log_add_sta_msg(ngm);
		break;

	case NSS_GRE_REDIR_LAG_DS_DEL_STA_MSG:
		nss_gre_redir_lag_ds_log_del_sta_msg(ngm);
		break;

	case NSS_GRE_REDIR_LAG_DS_UPDATE_STA_MSG:
		nss_gre_redir_lag_ds_log_update_sta_msg(ngm);
		break;

	case NSS_GRE_REDIR_LAG_DS_STATS_SYNC_MSG:
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
 * nss_gre_redir_lag_ds_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_gre_redir_lag_ds_log_tx_msg(struct nss_gre_redir_lag_ds_msg *ngm)
{
	if (ngm->cm.type >= NSS_GRE_REDIR_LAG_DS_MAX_MSG_TYPES) {
		nss_warning("%px: Invalid message type\n", ngm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ngm, ngm->cm.type, nss_gre_redir_lag_ds_log_message_types_str[ngm->cm.type]);
	nss_gre_redir_lag_ds_log_verbose(ngm);
}

/*
 * nss_gre_redir_lag_ds_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_gre_redir_lag_ds_log_rx_msg(struct nss_gre_redir_lag_ds_msg *ngm)
{
	if (ngm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ngm);
		return;
	}

	if (ngm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ngm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ngm, ngm->cm.type,
			nss_gre_redir_lag_ds_log_message_types_str[ngm->cm.type],
			ngm->cm.response, nss_cmn_response_str[ngm->cm.response]);
		goto verbose;
	}

	if (ngm->cm.error >= NSS_GRE_REDIR_LAG_ERR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ngm, ngm->cm.type, nss_gre_redir_lag_ds_log_message_types_str[ngm->cm.type],
			ngm->cm.response, nss_cmn_response_str[ngm->cm.response],
			ngm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ngm, ngm->cm.type, nss_gre_redir_lag_ds_log_message_types_str[ngm->cm.type],
		ngm->cm.response, nss_cmn_response_str[ngm->cm.response],
		ngm->cm.error, nss_gre_redir_lag_ds_log_error_response_types_str[ngm->cm.error]);

verbose:
	nss_gre_redir_lag_ds_log_verbose(ngm);
}
