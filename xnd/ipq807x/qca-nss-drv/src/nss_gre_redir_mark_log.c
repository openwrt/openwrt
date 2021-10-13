/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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

#include "nss_core.h"

/*
 * nss_gre_redir_mark_log_message_types_str
 *	GRE redir mark message strings
 */
static int8_t *nss_gre_redir_mark_log_message_types_str[NSS_GRE_REDIR_MARK_MSG_MAX] __maybe_unused = {
	"GRE redir mark register callback message",
	"GRE redir mark statistics synchronization"
};

/*
 * nss_gre_redir_mark_log_error_response_types_str
 *	Strings for error types for GRE redir mark messages
 */
static int8_t *nss_gre_redir_mark_log_error_response_types_str[NSS_GRE_REDIR_MARK_ERROR_TYPE_MAX] __maybe_unused = {
	"GRE redir mark No error",
	"GRE redir mark Invalid interface for callback registration",
	"GRE redir mark Invalid ethertype for Tx interface"
};

/*
 * nss_gre_redir_mark_log_reg_cb_msg()
 *	Log NSS GRE redir mark configuration message
 */
static void nss_gre_redir_mark_log_reg_cb_msg(struct nss_gre_redir_mark_msg *ncm)
{
	struct nss_gre_redir_mark_register_cb_msg *reg_cb_msg __maybe_unused = &ncm->msg.reg_cb_msg;
	nss_trace("%px: NSS GRE redir mark callback registration message \n"
			"nss_if_num: %d\n", ncm, reg_cb_msg->nss_if_num);
}

/*
 * nss_gre_redir_mark_log_verbose()
 *	Log message contents.
 */
static void nss_gre_redir_mark_log_verbose(struct nss_gre_redir_mark_msg *ncm)
{
	switch (ncm->cm.type) {
	case NSS_GRE_REDIR_MARK_REG_CB_MSG:
		nss_gre_redir_mark_log_reg_cb_msg(ncm);
		break;

	case NSS_GRE_REDIR_MARK_STATS_SYNC_MSG:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_trace("%px: Invalid message type\n", ncm);
		break;
	}
}

/*
 * nss_gre_redir_mark_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_gre_redir_mark_log_tx_msg(struct nss_gre_redir_mark_msg *ngm)
{
	if (ngm->cm.type >= NSS_GRE_REDIR_MARK_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", ngm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ngm, ngm->cm.type, nss_gre_redir_mark_log_message_types_str[ngm->cm.type]);
	nss_gre_redir_mark_log_verbose(ngm);
}
/*
 * nss_gre_redir_mark_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_gre_redir_mark_log_rx_msg(struct nss_gre_redir_mark_msg *ncm)
{
	if (ncm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ncm);
		return;
	}

	if (ncm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ncm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ncm, ncm->cm.type,
			nss_gre_redir_mark_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response]);
		goto verbose;
	}

	if (ncm->cm.error >= NSS_GRE_REDIR_MARK_ERROR_TYPE_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ncm, ncm->cm.type, nss_gre_redir_mark_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
			ncm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ncm, ncm->cm.type, nss_gre_redir_mark_log_message_types_str[ncm->cm.type],
		ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
		ncm->cm.error, nss_gre_redir_mark_log_error_response_types_str[ncm->cm.error]);

verbose:
	nss_gre_redir_mark_log_verbose(ncm);
}
