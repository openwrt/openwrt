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
 * nss_map_t_log.c
 *	NSS MAP_T logger file.
 */

#include "nss_core.h"

/*
 * nss_map_t_log_message_types_str
 *	NSS MAP_T message strings
 */
static int8_t *nss_map_t_log_message_types_str[NSS_MAP_T_MSG_MAX] __maybe_unused = {
	"MAP_T Rule Configure",
	"MAP_T Rule Deconfigure",
	"MAP_T Stats",
};

/*
 * nss_map_t_log_rule_configure_msg()
 *	Log NSS MAP_T Rule Configure.
 */
static void nss_map_t_log_rule_configure_msg(struct nss_map_t_msg *ntm)
{
	struct nss_map_t_instance_rule_config_msg *ntcm __maybe_unused = &ntm->msg.create_msg;
	nss_trace("%px: NSS MAP_T Rule Configure message \n"
		"MAP_T Rule Seq Number: %d\n"
		"MAP_T Total Number of Rules: %d\n"
		"MAP_T Local IPv6 Prefix Length: %d\n"
		"MAP_T Local IPv4 Prefix: %d\n"
		"MAP_T Local IPv4 Prefix Length: %d\n"
		"MAP_T Local EA Bits Length: %d\n"
		"MAP_T Local PSID Offset: %d\n"
		"MAP_T Reserved A: %d\n"
		"MAP_T Remote IPv6 Prefix Length: %d\n"
		"MAP_T Remote IPv4 Prefix: %d\n"
		"MAP_T Remote IPv4 Prefix Length: %d\n"
		"MAP_T Remote EA Bits Length: %d\n"
		"MAP_T Remote PSID Offset: %d\n"
		"MAP_T Local MAP Style: %d\n"
		"MAP_T Remote Map Style: %d\n"
		"MAP_T Local IPv6 Prefix: %px\n"
		"MAP_T Reserved B: %px\n"
		"MAP_T Remote IPv6 Prefix: %px\n"
		"MAP_T Valid Rule: %d\n",
		ntcm, ntcm->rule_num, ntcm->total_rules,
		ntcm->local_ipv6_prefix_len, ntcm->local_ipv4_prefix,
		ntcm->local_ipv4_prefix_len, ntcm->local_ea_len,
		ntcm->local_psid_offset, ntcm->reserve_a,
		ntcm->remote_ipv6_prefix_len,
		ntcm->remote_ipv4_prefix, ntcm->remote_ipv4_prefix_len,
		ntcm->remote_ea_len, ntcm->remote_psid_offset,
		ntcm->local_map_style, ntcm->remote_map_style,
		ntcm->local_ipv6_prefix, ntcm->reserve_b,
		ntcm->remote_ipv6_prefix, ntcm->valid_rule);
}

/*
 * nss_map_t_log_rule_deconfig_msg()
 *	Log NSS MAP_T Rule Deconfigure.
 */
static void nss_map_t_log_rule_deconfig_msg(struct nss_map_t_msg *ntm)
{
	struct nss_map_t_instance_rule_deconfig_msg *ntdm __maybe_unused = &ntm->msg.destroy_msg;
	nss_trace("%px: NSS MAP_T Rule Deconfigure message \n"
		"MAP_T Interface Number: %d\n",
		ntdm, ntdm->if_number);
}

/*
 * nss_map_t_log_verbose()
 *	Log message contents.
 */
static void nss_map_t_log_verbose(struct nss_map_t_msg *ntm)
{
	switch (ntm->cm.type) {
	case NSS_MAP_T_MSG_INSTANCE_RULE_CONFIGURE:
		nss_map_t_log_rule_configure_msg(ntm);
		break;

	case NSS_MAP_T_MSG_INSTANCE_RULE_DECONFIGURE:
		nss_map_t_log_rule_deconfig_msg(ntm);
		break;

	case NSS_MAP_T_MSG_SYNC_STATS:
		/*
		 * No log for valid stats message.
		 */
		break;

	default:
		nss_trace("%px: Invalid message type\n", ntm);
		break;
	}
}

/*
 * nss_map_t_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_map_t_log_tx_msg(struct nss_map_t_msg *ntm)
{
	if (ntm->cm.type >= NSS_MAP_T_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", ntm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ntm, ntm->cm.type, nss_map_t_log_message_types_str[ntm->cm.type]);
	nss_map_t_log_verbose(ntm);
}

/*
 * nss_map_t_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_map_t_log_rx_msg(struct nss_map_t_msg *ntm)
{
	if (ntm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ntm);
		return;
	}

	if (ntm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ntm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ntm, ntm->cm.type,
			nss_map_t_log_message_types_str[ntm->cm.type],
			ntm->cm.response, nss_cmn_response_str[ntm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		ntm, ntm->cm.type, nss_map_t_log_message_types_str[ntm->cm.type],
		ntm->cm.response, nss_cmn_response_str[ntm->cm.response]);

verbose:
	nss_map_t_log_verbose(ntm);
}
