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
 * nss_ppe_log.c
 *	NSS PPE logger file.
 */

#include "nss_core.h"
#include "nss_ppe.h"

/*
 * nss_ppe_log_message_types_str
 *	PPE message strings
 */
static int8_t *nss_ppe_log_message_types_str[NSS_PPE_MSG_MAX] __maybe_unused = {
	"PPE Stats",
	"PPE IPSEC Port Config",
	"PPE IPSEC Port MTU Change",
	"PPE IPSEC Add Interface",
	"PPE IPSEC Del Interface",
};

/*
 * nss_ppe_log_error_response_types_str
 *	Strings for error types for PPE messages
 */
static int8_t *nss_ppe_log_error_response_types_str[PPE_ERROR_MAX] __maybe_unused = {
	"PPE No Error",
	"PPE Uknown Type",
	"PPE Port Creation Failure",
	"PPE Invalid Port VSI",
	"PPE Invalid L3 Interface",
	"PPE IPSEC Port Config Error",
	"PPE IPSEC Interface Table Full",
	"PPE IPSEC Interface Attached",
	"PPE IPSEC Interface Unattached",
};

/*
 * nss_ppe_log_port_config_msg()
 *	Log NSS PPE port config message.
 */
static void nss_ppe_log_port_config_msg(struct nss_ppe_msg *npm)
{
	struct nss_ppe_ipsec_port_config_msg *npcm __maybe_unused = &npm->msg.ipsec_config;
	nss_trace("%px: NSS PPE Port Configure Message:\n"
		"PPE NSS Interface Number: %d\n"
		"PPE MTU: %d\n"
		"PPE VSI Number: %d\n",
		npcm, npcm->nss_ifnum,
		npcm->mtu, npcm->vsi_num);
}

/*
 * nss_ppe_log_port_mtu_msg()
 *	Log NSS PPE port mtu message.
 */
static void nss_ppe_log_port_mtu_msg(struct nss_ppe_msg *npm)
{
	struct nss_ppe_ipsec_port_mtu_msg *npmm __maybe_unused = &npm->msg.ipsec_mtu;
	nss_trace("%px: NSS PPE Port Configure Message:\n"
		"PPE NSS Interface Number: %d\n"
		"PPE MTU: %d\n",
		npmm, npmm->nss_ifnum,
		npmm->mtu);
}

/*
 * nss_ppe_log_add_intf_msg()
 *	Log NSS PPE IPSEC Add Interface Message.
 */
static void nss_ppe_log_add_intf_msg(struct nss_ppe_msg *npm)
{
	struct nss_ppe_ipsec_add_intf_msg *npam __maybe_unused = &npm->msg.ipsec_addif;
	nss_trace("%px: NSS PPE IPSEC add Interface Message:\n"
		"PPE NSS Interface Number: %d\n",
		npam, npam->nss_ifnum);
}

/*
 * nss_ppe_log_del_intf_msg()
 *	Log NSS PPE IPSEC Delete Interface Message.
 */
static void nss_ppe_log_del_intf_msg(struct nss_ppe_msg *npm)
{
	struct nss_ppe_ipsec_del_intf_msg *npdm __maybe_unused = &npm->msg.ipsec_delif;
	nss_trace("%px: NSS PPE IPSEC Delete Interface Message:\n"
		"PPE NSS Interface Number: %d\n",
		npdm, npdm->nss_ifnum);
}

/*
 * nss_ppe_log_verbose()
 *	Log message contents.
 */
static void nss_ppe_log_verbose(struct nss_ppe_msg *npm)
{
	switch (npm->cm.type) {
	case NSS_PPE_MSG_IPSEC_PORT_CONFIG:
		nss_ppe_log_port_config_msg(npm);
		break;

	case NSS_PPE_MSG_IPSEC_PORT_MTU_CHANGE:
		nss_ppe_log_port_mtu_msg(npm);
		break;

	case NSS_PPE_MSG_IPSEC_ADD_INTF:
		nss_ppe_log_add_intf_msg(npm);
		break;

	case NSS_PPE_MSG_IPSEC_DEL_INTF:
		nss_ppe_log_del_intf_msg(npm);
		break;

	case NSS_PPE_MSG_SYNC_STATS:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_warning("%px: Invalid message type\n", npm);
		break;
	}
}

/*
 * nss_ppe_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_ppe_log_tx_msg(struct nss_ppe_msg *npm)
{
	if (npm->cm.type >= NSS_PPE_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", npm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", npm, npm->cm.type, nss_ppe_log_message_types_str[npm->cm.type]);
	nss_ppe_log_verbose(npm);
}

/*
 * nss_ppe_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_ppe_log_rx_msg(struct nss_ppe_msg *npm)
{
	if (npm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", npm);
		return;
	}

	if (npm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (npm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", npm, npm->cm.type,
			nss_ppe_log_message_types_str[npm->cm.type],
			npm->cm.response, nss_cmn_response_str[npm->cm.response]);
		goto verbose;
	}

	if (npm->cm.error >= PPE_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			npm, npm->cm.type, nss_ppe_log_message_types_str[npm->cm.type],
			npm->cm.response, nss_cmn_response_str[npm->cm.response],
			npm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		npm, npm->cm.type, nss_ppe_log_message_types_str[npm->cm.type],
		npm->cm.response, nss_cmn_response_str[npm->cm.response],
		npm->cm.error, nss_ppe_log_error_response_types_str[npm->cm.error]);

verbose:
	nss_ppe_log_verbose(npm);
}
