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
 * nss_gre_redir_log.c
 *	NSS GRE REDIR logger file.
 */

#include "nss_core.h"

/*
 * nss_gre_redir_log_message_types_str
 *	NSS GRE REDIR message strings
 */
static int8_t *nss_gre_redir_log_message_types_str[NSS_GRE_REDIR_MAX_MSG_TYPES] __maybe_unused = {
	"GRE REDIR Tunnel Inner Configure",
	"GRE REDIR Tunnel Outer Configure",
	"GRE REDIR Interface Map",
	"GRE REDIR Interface Unmap",
	"GRE REDIR SJACK Map",
	"GRE REDIR SJACK Unmap",
	"GRE REDIR Stats Sync",
	"GRE REDIR Exception DS register cb"
};

/*
 * nss_gre_redir_log_inner_configure_msg()
 *	Log NSS GRE Redir inner configure message.
 */
static void nss_gre_redir_log_inner_configure_msg(struct nss_gre_redir_msg *ngm)
{
	struct nss_gre_redir_inner_configure_msg *ngicm __maybe_unused = &ngm->msg.inner_configure;
	nss_trace("%px: NSS GRE Redir Inner Configure message"
		"GRE REDIR IP Header Type: %d\n"
		"GRE REDIR Source IP: %px\n"
		"GRE REDIR Destination IP: %px\n"
		"GRE REDIR Outer Interface: %d\n"
		"GRE REDIR Do not Fragment: %d\n"
		"GRE REDIR IP TTL: %d\n"
		"GRE REDIR Version: %d\n",
		ngicm, ngicm->ip_hdr_type,
		ngicm->ip_src_addr, ngicm->ip_dest_addr,
		ngicm->except_outerif, ngicm->ip_df_policy,
		ngicm->ip_ttl, ngicm->gre_version);
}

/*
 * nss_gre_redir_log_interface_map_msg()
 *	Log NSS GRE Redir interface map message.
 */
static void nss_gre_redir_log_interface_map_msg(struct nss_gre_redir_msg *ngm)
{
	struct nss_gre_redir_interface_map_msg *ngicm __maybe_unused = &ngm->msg.interface_map;
	nss_trace("%px: NSS GRE Redir Interface Map message"
		"GRE REDIR NSS VAP Interface: %d\n"
		"GRE REDIR Next Hop NSS Interface: %d\n"
		"GRE REDIR Radio ID: %d\n"
		"GRE REDIR VAP ID: %d\n"
		"GRE REDIR LAG Flags: %x\n"
		"GRE REDIR Tunnel Type: %d\n"
		"GRE REDIR IPsec pattern: %d\n",
		ngicm, ngicm->vap_nssif,
		ngicm->nexthop_nssif, ngicm->radio_id,
		ngicm->vap_id, ngicm->lag_en,
		ngicm->tunnel_type, ngicm->ipsec_pattern);
}

/*
 * nss_gre_redir_log_interface_unmap_msg()
 *	Log NSS GRE Redir interface unmap message.
 */
static void nss_gre_redir_log_interface_unmap_msg(struct nss_gre_redir_msg *ngm)
{
	struct nss_gre_redir_interface_unmap_msg *ngicm __maybe_unused = &ngm->msg.interface_unmap;
	nss_trace("%px: NSS GRE Redir Interface Map message"
		"GRE REDIR NSS VAP Interface: %d\n"
		"GRE REDIR Radio ID: %d\n"
		"GRE REDIR VAP ID: %d\n",
		ngicm, ngicm->vap_nssif,
		ngicm->radio_id, ngicm->vap_id);
}

/*
 * nss_gre_redir_log_sjack_map_msg()
 *	Log NSS GRE Redir interface map message.
 */
static void nss_gre_redir_log_sjack_map_msg(struct nss_gre_redir_msg *ngm)
{
	struct nss_gre_redir_sjack_map_msg *ngscm __maybe_unused = &ngm->msg.sjack_map;
	nss_trace("%px: NSS GRE Redir SJACK Map message"
		"GRE REDIR Eth NSS Interface: %d\n"
		"GRE REDIR Eth Interface ID: %d\n"
		"GRE REDIR IPSec pattern: %x\n",
		ngscm, ngscm->eth_nssif,
		ngscm->eth_id, ngscm->ipsec_pattern);
}

/*
 * nss_gre_redir_log_sjack_unmap_msg()
 *	Log NSS GRE Redir interface unmap message.
 */
static void nss_gre_redir_log_sjack_unmap_msg(struct nss_gre_redir_msg *ngm)
{
	struct nss_gre_redir_sjack_unmap_msg *ngscm __maybe_unused = &ngm->msg.sjack_unmap;
	nss_trace("%px: NSS GRE Redir SJACK Map message"
		"GRE REDIR Eth NSS Interface: %d\n"
		"GRE REDIR Eth Interface ID: %d\n",
		ngscm, ngscm->eth_nssif,
		ngscm->eth_id);
}

/*
 * nss_gre_redir_log_outer_configure_msg()
 *	Log NSS GRE Redir outer configure message.
 */
static void nss_gre_redir_log_outer_configure_msg(struct nss_gre_redir_msg *ngm)
{
	struct nss_gre_redir_outer_configure_msg *ngocm __maybe_unused = &ngm->msg.outer_configure;
	nss_trace("%px: NSS GRE Redir Outer Configure message"
		"GRE REDIR IP Header Type: %d\n"
		"GRE REDIR Host Inner Interface: %d\n"
		"GRE REDIR NSS Inner Interface: %d\n"
		"GRE REDIR SJACK Inner Interface: %d\n"
		"GRE REDIR RPS: %d\n"
		"GRE REDIR RPS Valid: %d\n",
		ngocm, ngocm->ip_hdr_type,
		ngocm->except_hostif, ngocm->except_offlif,
		ngocm->except_sjackif, ngocm->rps_hint,
		ngocm->rps_hint_valid);
}

/*
 * nss_gre_redir_log_exception_ds_reg_cb_msg()
 *	Log GRE exception downstream callback registration message.
 */
static void nss_gre_redir_log_exception_ds_reg_cb_msg(struct nss_gre_redir_msg *ngm)
{
	struct nss_gre_redir_exception_ds_reg_cb_msg *exception_ds_configure __maybe_unused = &ngm->msg.exception_ds_configure;
	nss_trace("%px: NSS GRE redir exception completion callback registration message\n"
			"vap_if_num: %d\n", ngm, exception_ds_configure->dst_vap_nssif);
}

/*
 * nss_gre_redir_log_verbose()
 *	Log message contents.
 */
static void nss_gre_redir_log_verbose(struct nss_gre_redir_msg *ngm)
{
	switch (ngm->cm.type) {
	case NSS_GRE_REDIR_TX_TUNNEL_INNER_CONFIGURE_MSG:
		nss_gre_redir_log_inner_configure_msg(ngm);
		break;

	case NSS_GRE_REDIR_TX_TUNNEL_OUTER_CONFIGURE_MSG:
		nss_gre_redir_log_outer_configure_msg(ngm);
		break;

	case NSS_GRE_REDIR_TX_INTERFACE_MAP_MSG:
		nss_gre_redir_log_interface_map_msg(ngm);
		break;

	case NSS_GRE_REDIR_TX_INTERFACE_UNMAP_MSG:
		nss_gre_redir_log_interface_unmap_msg(ngm);
		break;

	case NSS_GRE_REDIR_TX_SJACK_MAP_MSG:
		nss_gre_redir_log_sjack_map_msg(ngm);
		break;

	case NSS_GRE_REDIR_TX_SJACK_UNMAP_MSG:
		nss_gre_redir_log_sjack_unmap_msg(ngm);
		break;

	case NSS_GRE_REDIR_RX_STATS_SYNC_MSG:
		/*
		 * No log for valid stats message.
		 */
		break;

	case NSS_GRE_REDIR_EXCEPTION_DS_REG_CB_MSG:
		nss_gre_redir_log_exception_ds_reg_cb_msg(ngm);
		break;

	default:
		nss_warning("%px: Invalid message type\n", ngm);
		break;
	}
}

/*
 * nss_gre_redir_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_gre_redir_log_tx_msg(struct nss_gre_redir_msg *ngm)
{
	if (ngm->cm.type >= NSS_GRE_REDIR_MAX_MSG_TYPES) {
		nss_warning("%px: Invalid message type\n", ngm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ngm, ngm->cm.type, nss_gre_redir_log_message_types_str[ngm->cm.type]);
	nss_gre_redir_log_verbose(ngm);
}

/*
 * nss_gre_redir_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_gre_redir_log_rx_msg(struct nss_gre_redir_msg *ngm)
{
	if (ngm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ngm);
		return;
	}

	if (ngm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ngm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ngm, ngm->cm.type,
			nss_gre_redir_log_message_types_str[ngm->cm.type],
			ngm->cm.response, nss_cmn_response_str[ngm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		ngm, ngm->cm.type, nss_gre_redir_log_message_types_str[ngm->cm.type],
		ngm->cm.response, nss_cmn_response_str[ngm->cm.response]);

verbose:
	nss_gre_redir_log_verbose(ngm);
}
