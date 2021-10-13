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
 * nss_bridge_log.c
 *	NSS Bridge logger file.
 */

#include "nss_core.h"

/*
 * nss_bridge_log_message_types_str
 *	NSS Bridge message strings
 */
static int8_t *nss_bridge_log_message_types_str[NSS_BRIDGE_MSG_TYPE_MAX] __maybe_unused = {
	"Bridge Join message",
	"Bridge Leave message",
	"Bridge Set FDB Learn message"
};

/*
 * nss_bridge_join_msg()
 *	Log NSS Bridge Join message.
 */
static void nss_bridge_join_msg(struct nss_bridge_msg *nbm)
{
	struct nss_bridge_join_msg *nbjm __maybe_unused = &nbm->msg.br_join;
	nss_trace("%px: NSS Bridge Join message \n"
		"Interface Number: %d\n",
		nbm, nbjm->if_num);
}

/*
 * nss_bridge_leave_msg()
 *	Log NSS Bridge Leave message.
 */
static void nss_bridge_leave_msg(struct nss_bridge_msg *nbm)
{
	struct nss_bridge_leave_msg *nblm __maybe_unused = &nbm->msg.br_leave;
	nss_trace("%px: NSS Bridge Leave message: \n"
		"Interface Number: %d\n",
		nbm, nblm->if_num);
}

/*
 * nss_bridge_fdb_learn_msg()
 *	Log NSS Set Bridge FDB Learn message.
 */
static void nss_bridge_fdb_learn_msg(struct nss_bridge_msg *nbm)
{
	struct nss_bridge_set_fdb_learn_msg *nbflm __maybe_unused =
		&nbm->msg.fdb_learn;
	nss_trace("%px: NSS Bridge Set FDB Learn message: \n"
		"Mode: %d\n",
		nbm, nbflm->mode);
}

/*
 * nss_bridge_log_verbose()
 *	Log message contents.
 */
static void nss_bridge_log_verbose(struct nss_bridge_msg *nbm)
{
	switch (nbm->cm.type) {
	case NSS_BRIDGE_MSG_JOIN:
		nss_bridge_join_msg(nbm);
		break;

	case NSS_BRIDGE_MSG_LEAVE:
		nss_bridge_leave_msg(nbm);
		break;

	case NSS_BRIDGE_MSG_SET_FDB_LEARN:
		nss_bridge_fdb_learn_msg(nbm);
		break;

	default:
		nss_trace("%px: Invalid message type\n", nbm);
		break;
	}
}

/*
 * nss_bridge_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_bridge_log_tx_msg(struct nss_bridge_msg *nbm)
{
	if (nbm->cm.type >= NSS_BRIDGE_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", nbm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nbm, nbm->cm.type, nss_bridge_log_message_types_str[nbm->cm.type - NSS_IF_MAX_MSG_TYPES - 1]);
	nss_bridge_log_verbose(nbm);
}

/*
 * nss_bridge_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_bridge_log_rx_msg(struct nss_bridge_msg *nbm)
{
	if (nbm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nbm);
		return;
	}

	if (nbm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nbm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nbm, nbm->cm.type,
			nss_bridge_log_message_types_str[nbm->cm.type - NSS_IF_MAX_MSG_TYPES - 1],
			nbm->cm.response, nss_cmn_response_str[nbm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		nbm, nbm->cm.type, nss_bridge_log_message_types_str[nbm->cm.type - NSS_IF_MAX_MSG_TYPES - 1],
		nbm->cm.response, nss_cmn_response_str[nbm->cm.response]);

verbose:
	nss_bridge_log_verbose(nbm);
}
