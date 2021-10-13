/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
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
 * nss_mirror_log.c
 *	NSS Mirror logger file.
 */

#include "nss_core.h"

/*
 * nss_mirror_log_message_types_str
 *	MIRROR message strings
 */
static int8_t *nss_mirror_log_message_types_str[NSS_MIRROR_MSG_MAX] __maybe_unused = {
	"Mirror Configure Msg",
	"Mirror Enable Msg",
	"Mirror Disable Msg",
	"Mirror Set Nexthop Msg",
	"Mirror Reset Nexthop Msg",
	"Mirror Stats Sync Msg",
};

/*
 * nss_mirror_log_error_response_types_str
 *	Strings for error types for Mirror messages
 */
static int8_t *nss_mirror_log_error_response_types_str[NSS_MIRROR_ERROR_TYPE_MAX] __maybe_unused = {
	"Mirror no error",
	"Mirror No Memory",
	"Mirror Transmit Failure",
	"Mirror Bad Parameter",
	"Mirror Bad Clone Point",
	"Mirror Intance Configured",
	"Mirror Intance Disabled",
	"Mirror Bad Nexthop",
	"Mirror Nexthop Configured",
	"Mirror Nexthop Reset",
	"Mirror Unknown Message",
};

/*
 * nss_mirror_log_configure_msg()
 *	Log NSS Mirror Configure message.
 */
static void nss_mirror_log_configure_msg(struct nss_mirror_msg *nmm)
{
	struct nss_mirror_configure_msg *config_msg __maybe_unused = &nmm->msg.config;

	nss_trace("%px: NSS Mirror Config message \n"
		"Packet clone size: %u\n"
		"Packet clone point: %hu\n",
		config_msg,
		config_msg->pkt_clone_size,
		config_msg->pkt_clone_point);
}

/*
 * nss_mirror_log_set_nexthop_msg()
 *	Log NSS Mirror Set Nexthop message.
 */
static void nss_mirror_log_set_nexthop_msg(struct nss_mirror_msg *nmm)
{
	struct nss_mirror_set_nexthop_msg *nexthop_msg __maybe_unused = &nmm->msg.nexthop;

	nss_trace("%px: NSS Mirror Nexthop message \n"
		"Nexthop interface number: %u\n",
		nexthop_msg,
		nexthop_msg->if_num);
}

/*
 * nss_mirror_log_enable_msg()
 *	Log NSS Mirror Enable message.
 */
static void nss_mirror_log_enable_msg(struct nss_mirror_msg *nmm)
{
	nss_trace("%px: NSS Mirror message: Enable \n", nmm);
}

/*
 * nss_mirror_log_disable_msg()
 *	Log NSS Mirror Disable message.
 */
static void nss_mirror_log_disable_msg(struct nss_mirror_msg *nmm)
{
	nss_trace("%px: NSS Mirror message: Disable \n", nmm);
}

/*
 * nss_mirror_log_reset_nexthop_msg()
 *	Log NSS Mirror Reset Nexthop message.
 */
static void nss_mirror_log_reset_nexthop_msg(struct nss_mirror_msg *nmm)
{
	nss_trace("%px: NSS Mirror message: Reset Nexthop \n", nmm);
}

/*
 * nss_mirror_log_verbose()
 *	Log message contents.
 */
static void nss_mirror_log_verbose(struct nss_mirror_msg *nmm)
{
	switch (nmm->cm.type) {
	case NSS_MIRROR_MSG_CONFIGURE:
		nss_mirror_log_configure_msg(nmm);
		break;

	case NSS_MIRROR_MSG_ENABLE:
		nss_mirror_log_enable_msg(nmm);
		break;

	case NSS_MIRROR_MSG_DISABLE:
		nss_mirror_log_disable_msg(nmm);
		break;

	case NSS_MIRROR_MSG_SET_NEXTHOP:
		nss_mirror_log_set_nexthop_msg(nmm);
		break;

	case NSS_MIRROR_MSG_RESET_NEXTHOP:
		nss_mirror_log_reset_nexthop_msg(nmm);
		break;

	case NSS_MIRROR_MSG_SYNC_STATS:
		break;

	default:
		nss_trace("%px: Invalid message type\n", nmm);
		break;
	}
}

/*
 * nss_mirror_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_mirror_log_tx_msg(struct nss_mirror_msg *nmm)
{
	if (nmm->cm.type >= NSS_MIRROR_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", nmm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nmm, nmm->cm.type, nss_mirror_log_message_types_str[nmm->cm.type]);
	nss_mirror_log_verbose(nmm);
}

/*
 * nss_mirror_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_mirror_log_rx_msg(struct nss_mirror_msg *nmm)
{
	if (nmm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nmm);
		return;
	}

	if (nmm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nmm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nmm, nmm->cm.type,
			nss_mirror_log_message_types_str[nmm->cm.type],
			nmm->cm.response, nss_cmn_response_str[nmm->cm.response]);
		goto verbose;
	}

	if (nmm->cm.error >= NSS_MIRROR_ERROR_TYPE_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nmm, nmm->cm.type, nss_mirror_log_message_types_str[nmm->cm.type],
			nmm->cm.response, nss_cmn_response_str[nmm->cm.response],
			nmm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nmm, nmm->cm.type, nss_mirror_log_message_types_str[nmm->cm.type],
		nmm->cm.response, nss_cmn_response_str[nmm->cm.response],
		nmm->cm.error, nss_mirror_log_error_response_types_str[nmm->cm.error]);

verbose:
	nss_mirror_log_verbose(nmm);
}
