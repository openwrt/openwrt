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
 * nss_sjack_log.c
 *	NSS SJACK logger file.
 */

#include "nss_core.h"

/*
 * nss_sjack_log_message_types_str
 *	NSS SJACK message strings
 */
static int8_t *nss_sjack_log_message_types_str[NSS_SJACK_MAX_MSG_TYPE] __maybe_unused = {
	"SJACK Configure",
	"SJACK Unconfigure",
	"SJACK Stats",
};

/*
 * nss_sjack_log_configure_msg()
 *	Log NSS SJACK Configure.
 */
static void nss_sjack_log_configure_msg(struct nss_sjack_msg *nsm)
{
	struct nss_sjack_configure_msg *nscm __maybe_unused = &nsm->msg.configure;
	nss_trace("%px: NSS SJACK Configure message \n"
		"SJACK Ingress Interface Number: %d\n"
		"SJACK Engress Interface Number: %d\n"
		"SJACK Tunnel ID: %d\n"
		"SJACK DSCP Value: %d\n"
		"SJACK GRE Priority: %d\n"
		"SJACK GRE Flags: %d\n"
		"SJACK IPSEC SA Pattern Flag: %d\n",
		nscm, nscm->ingress_if_num,
		nscm->egress_if_num, nscm->tunnel_id,
		nscm->ip_dscp, nscm->gre_prio,
		nscm->gre_flags, nscm->use_ipsec_sa_pattern);
}

/*
 * nss_sjack_log_unconfigure_msg()
 *	Log NSS SJACK Unconfigure.
 */
static void nss_sjack_log_unconfigure_msg(struct nss_sjack_msg *nsm)
{
	struct nss_sjack_unconfigure_msg *nsum __maybe_unused = &nsm->msg.unconfigure;
	nss_trace("%px: NSS SJACK UnConfigure message \n"
		"SJACK Ingress Interface Number: %d\n",
		nsum, nsum->ingress_if_num);
}

/*
 * nss_sjack_log_verbose()
 *	Log message contents.
 */
static void nss_sjack_log_verbose(struct nss_sjack_msg *nsm)
{
	switch (nsm->cm.type) {
	case NSS_SJACK_CONFIGURE_MSG:
		nss_sjack_log_configure_msg(nsm);
		break;

	case NSS_SJACK_UNCONFIGURE_MSG:
		nss_sjack_log_unconfigure_msg(nsm);
		break;

	case NSS_SJACK_STATS_SYNC_MSG:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_trace("%px: Invalid message type\n", nsm);
		break;
	}
}

/*
 * nss_sjack_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_sjack_log_tx_msg(struct nss_sjack_msg *nsm)
{
	if (nsm->cm.type >= NSS_SJACK_MAX_MSG_TYPE) {
		nss_warning("%px: Invalid message type\n", nsm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nsm, nsm->cm.type, nss_sjack_log_message_types_str[nsm->cm.type]);
	nss_sjack_log_verbose(nsm);
}

/*
 * nss_sjack_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_sjack_log_rx_msg(struct nss_sjack_msg *nsm)
{
	if (nsm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nsm);
		return;
	}

	if (nsm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nsm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nsm, nsm->cm.type,
			nss_sjack_log_message_types_str[nsm->cm.type],
			nsm->cm.response, nss_cmn_response_str[nsm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		nsm, nsm->cm.type, nss_sjack_log_message_types_str[nsm->cm.type],
		nsm->cm.response, nss_cmn_response_str[nsm->cm.response]);

verbose:
	nss_sjack_log_verbose(nsm);
}
