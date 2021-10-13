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
 * nss_oam_log.c
 *	NSS OAM logger file.
 */

#include "nss_core.h"

/*
 * nss_oam_log_message_types_str
 *	NSS OAM message strings
 */
static int8_t *nss_oam_log_message_types_str[NSS_OAM_MSG_TYPE_MAX] __maybe_unused = {
	"OAM Message None",
	"OAM Get FW Version",
};

/*
 * nss_oam_log_get_fw_version_msg()
 *	Log NSS OAM GET FW Version.
 */
static void nss_oam_log_get_fw_version_msg(struct nss_oam_msg *nom)
{
	struct nss_oam_fw_ver *nofm __maybe_unused = &nom->msg.fw_ver;
	nss_trace("%px: NSS OAM Get FW Version message \n"
		"OAM FW Version: %px\n",
		nofm, nofm->string);
}

/*
 * nss_oam_log_verbose()
 *	Log message contents.
 */
static void nss_oam_log_verbose(struct nss_oam_msg *nom)
{
	switch (nom->cm.type) {
	case NSS_OAM_MSG_TYPE_GET_FW_VER:
		nss_oam_log_get_fw_version_msg(nom);
		break;

	default:
		nss_trace("%px: Invalid message type\n", nom);
		break;
	}
}

/*
 * nss_oam_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_oam_log_tx_msg(struct nss_oam_msg *nom)
{
	if (nom->cm.type >= NSS_OAM_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type\n", nom);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nom, nom->cm.type, nss_oam_log_message_types_str[nom->cm.type]);
	nss_oam_log_verbose(nom);
}

/*
 * nss_oam_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_oam_log_rx_msg(struct nss_oam_msg *nom)
{
	if (nom->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nom);
		return;
	}

	if (nom->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nom->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nom, nom->cm.type,
			nss_oam_log_message_types_str[nom->cm.type],
			nom->cm.response, nss_cmn_response_str[nom->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		nom, nom->cm.type, nss_oam_log_message_types_str[nom->cm.type],
		nom->cm.response, nss_cmn_response_str[nom->cm.response]);

verbose:
	nss_oam_log_verbose(nom);
}
