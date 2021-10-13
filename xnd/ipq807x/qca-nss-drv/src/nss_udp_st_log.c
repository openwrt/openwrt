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
 * nss_udp_st_log.c
 *	NSS UDP Speedtest logger file.
 */

#include "nss_core.h"

/*
 * nss_udp_st_log_message_types_str
 *	udp_st message strings
 */
static int8_t *nss_udp_st_log_message_types_str[NSS_UDP_ST_MAX_MSG_TYPES] __maybe_unused = {
	"UDP_ST Start Msg",
	"UDP_ST Stop Msg",
	"UDP_ST Configure Rule Msg",
	"UDP_ST Unconfigure Rule Msg",
	"UDP_ST Stats Sync Msg",
	"UDP_ST TX Create Msg",
	"UDP_ST TX Destroy Msg",
	"UDP_ST Reset Stats Msg",
};

/*
 * nss_udp_st_log_error_response_types_str
 *	Strings for error types for udp_st messages
 */
static int8_t *nss_udp_st_log_error_response_types_str[NSS_UDP_ST_ERROR_MAX] __maybe_unused = {
	"UDP_ST No Error",
	"UDP_ST Incorrect Rate",
	"UDP_ST Incorrect Buffer Size",
	"UDP_ST Memory Failure",
	"UDP_ST Incorrect State",
	"UDP_ST Incorrect Flags",
	"UDP_ST Entry Exist",
	"UDP_ST Entry Add Failed",
	"UDP_ST Entry Not Exist",
	"UDP_ST Wrong Start Msg Type",
	"UDP_ST Wrong Stop Msg Type",
	"UDP_ST Too Many Users",
	"UDP_ST Unknown Msg Type",
	"UDP_ST Pbuf Alloc Failure",
	"UDP_ST Pbuf Size Failure",
	"UDP_ST Drop Queue",
	"UDP_ST Timer call missed",
};

/*
 * nss_udp_st_log_tx_create_destroy_msg()
 *	Log NSS udp_st Tx create/destroy message.
 */
static void nss_udp_st_log_tx_create_destroy_msg(struct nss_udp_st_msg *num, uint8_t *msg_type)
{
	struct nss_udp_st_tx_create *create __maybe_unused = &num->msg.create;
	nss_trace("%px: NSS udp_st message: %s\n"
		"Rate: %u\n"
		"Buffer Size: %u\n"
		"DSCP: %u\n",
		create,
		msg_type,
		create->rate,
		create->buffer_size,
		create->dscp);
}

/*
 * nss_udp_st_log_uncfg_rule_msg()
 *	Log NSS udp_st unconfig rule message.
 */
static void nss_udp_st_log_uncfg_rule_msg(struct nss_udp_st_msg *num)
{
	struct nss_udp_st_cfg *uncfg __maybe_unused = &num->msg.uncfg;
	nss_trace("%px: NSS udp_st message: Unconfig\n"
		"IP version: %u\n",
		uncfg,
		uncfg->ip_version);

	if (uncfg->ip_version == NSS_UDP_ST_FLAG_IPV4) {
		nss_trace("Src IP: %pI4\n"
			"Dest IP: %pI4\n",
			&(uncfg->src_ip.ip.ipv4),
			&(uncfg->dest_ip.ip.ipv4));
	} else {
		nss_trace("Src IP: %pI6\n"
			"Dest IP: %pI6\n",
			&(uncfg->src_ip.ip.ipv6),
			&(uncfg->dest_ip.ip.ipv6));
	}

	nss_trace("Src Port: %u\n Dest Port: %u\n Type: %u\n",
		uncfg->src_port, uncfg->dest_port, uncfg->type);
}

/*
 * nss_udp_st_log_cfg_rule_msg()
 *	Log NSS udp_st config rule message.
 */
static void nss_udp_st_log_cfg_rule_msg(struct nss_udp_st_msg *num)
{
	struct nss_udp_st_cfg *cfg __maybe_unused = &num->msg.cfg;
	nss_trace("%px: NSS udp_st message: Config\n"
		"IP version: %u\n",
		cfg,
		cfg->ip_version);

	if (cfg->ip_version == NSS_UDP_ST_FLAG_IPV4) {
		nss_trace("Src IP: %pI4\n"
			"Dest IP: %pI4\n",
			&(cfg->src_ip.ip.ipv4),
			&(cfg->dest_ip.ip.ipv4));
	} else {
		nss_trace("Src IP: %pI6\n"
			"Dest IP: %pI6\n",
			&(cfg->src_ip.ip.ipv6),
			&(cfg->dest_ip.ip.ipv6));
	}

	nss_trace("Src Port: %u\n Dest Port: %u\n Type: %u\n",
		cfg->src_port, cfg->dest_port, cfg->type);
}

/*
 * nss_udp_st_log_stop_msg()
 *	Log NSS udp_st stop message.
 */
static void nss_udp_st_log_stop_msg(struct nss_udp_st_msg *num)
{
	struct nss_udp_st_stop *stop __maybe_unused = &num->msg.stop;
	nss_trace("%px: NSS udp_st message: Stop\n"
		"Type: %u\n",
		stop,
		stop->type);
}

/*
 * nss_udp_st_log_start_msg()
 *	Log NSS udp_st start message.
 */
static void nss_udp_st_log_start_msg(struct nss_udp_st_msg *num)
{
	struct nss_udp_st_start *start __maybe_unused = &num->msg.start;
	nss_trace("%px: NSS udp_st message: Start\n"
		"Type: %u\n",
		start,
		start->type);
}

/*
 * nss_udp_st_log_verbose()
 *	Log message contents.
 */
static void nss_udp_st_log_verbose(struct nss_udp_st_msg *num)
{
	switch (num->cm.type) {
	case NSS_UDP_ST_START_MSG:
		nss_udp_st_log_start_msg(num);
		break;

	case NSS_UDP_ST_STOP_MSG:
		nss_udp_st_log_stop_msg(num);
		break;

	case NSS_UDP_ST_CFG_RULE_MSG:
		nss_udp_st_log_cfg_rule_msg(num);
		break;

	case NSS_UDP_ST_UNCFG_RULE_MSG:
		nss_udp_st_log_uncfg_rule_msg(num);
		break;

	case NSS_UDP_ST_TX_CREATE_MSG:
		nss_udp_st_log_tx_create_destroy_msg(num, "Create");
		break;

	case NSS_UDP_ST_TX_DESTROY_MSG:
		nss_udp_st_log_tx_create_destroy_msg(num, "Destroy");
		break;

	case NSS_UDP_ST_RESET_STATS_MSG:
	case NSS_UDP_ST_STATS_SYNC_MSG:
		break;

	default:
		nss_trace("%px: Invalid message type\n", num);
		break;
	}
}

/*
 * nss_udp_st_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_udp_st_log_tx_msg(struct nss_udp_st_msg *num)
{
	if (num->cm.type >= NSS_UDP_ST_MAX_MSG_TYPES) {
		nss_warning("%px: Invalid message type\n", num);
		return;
	}

	nss_info("%px: type[%d]:%s\n", num, num->cm.type, nss_udp_st_log_message_types_str[num->cm.type]);
	nss_udp_st_log_verbose(num);
}

/*
 * nss_udp_st_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_udp_st_log_rx_msg(struct nss_udp_st_msg *num)
{
	if (num->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", num);
		return;
	}

	if (num->cm.response == NSS_CMN_RESPONSE_NOTIFY || (num->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", num, num->cm.type,
			nss_udp_st_log_message_types_str[num->cm.type],
			num->cm.response, nss_cmn_response_str[num->cm.response]);
		goto verbose;
	}

	if (num->cm.error >= NSS_UDP_ST_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			num, num->cm.type, nss_udp_st_log_message_types_str[num->cm.type],
			num->cm.response, nss_cmn_response_str[num->cm.response],
			num->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		num, num->cm.type, nss_udp_st_log_message_types_str[num->cm.type],
		num->cm.response, nss_cmn_response_str[num->cm.response],
		num->cm.error, nss_udp_st_log_error_response_types_str[num->cm.error]);

verbose:
	nss_udp_st_log_verbose(num);
}
