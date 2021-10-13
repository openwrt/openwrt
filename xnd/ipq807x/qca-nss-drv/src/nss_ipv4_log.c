/*
 **************************************************************************
 * Copyright (c) 2016, 2018, 2020-2021, The Linux Foundation. All rights reserved.
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
 * nss_ipv4_log.c
 *	NSS IPv4 logger file.
 */

#include "nss_core.h"

/*
 * nss_ipv4_log_message_types_str
 *	IPv4 bridge/route rule message strings
 */
static int8_t *nss_ipv4_log_message_types_str[NSS_IPV4_MAX_MSG_TYPES] __maybe_unused = {
	"IPv4 create rule message",
	"IPv4 destroy rule message",
	"Deprecated: NSS_IPV4_RX_ESTABLISH_RULE_MSG",
	"IPv4 connection stats sync message",
	"IPv4 generic statistics sync message",
	"IPv4 number of connections supported rule message",
	"IPv4 multicast create rule message",
	"IPv4 request FW to send many conn sync message",
};

/*
 * nss_ipv4_log_error_response_types_str
 *	Strings for error types for ipv4 messages
 */
static int8_t *nss_ipv4_log_error_response_types_str[NSS_IPV4_LAST] __maybe_unused = {
	"No error",
	"Unknown error",
	"Invalid interface number",
	"Missing connection rule",
	"Buffer allocation failure",
	"No connection found to delete",
	"Conn cfg already done once",
	"Conn cfg input is not multiple of quanta",
	"Conn cfg input exceeds max supported connections",
	"Conn cfg mem alloc fail at NSS FW",
	"Invalid L4 protocol for multicast rule create",
	"Invalid multicast flags for multicast update",
	"Invalid interface for multicast update",
};

/*
 * nss_ipv4_log_rule_create_msg()
 *	Log IPv4 create rule message.
 */
static void nss_ipv4_log_rule_create_msg(struct nss_ipv4_msg *nim)
{
	struct nss_ipv4_rule_create_msg *nircm __maybe_unused = &nim->msg.rule_create;
	nss_trace("%px: IPv4 create rule message \n"
		"Protocol: %d\n"
		"from_mtu: %u\n"
		"to_mtu: %u\n"
		"from_ip: %pI4h:%d\n"
		"to_ip: %pI4h:%d\n"
		"from_ip_xlate: %pI4h:%d\n"
		"to_ip_xlate: %pI4h:%d\n"
		"from_mac: %pM\n"
		"to_mac: %pM\n"
		"src_iface_num: %u\n"
		"dest_iface_num: %u\n"
		"ingress_inner_vlan_tag: %u\n"
		"egress_inner_vlan_tag: %u\n"
		"ingress_outer_vlan_tag: %u\n"
		"egress_outer_vlan_tag: %u\n"
		"rule_flags: %x\n"
		"valid_flags: %x\n"
		"return_pppoe_if_exist: %u\n"
		"return_pppoe_if_num: %u\n"
		"flow_pppoe_if_exist: %u\n"
		"flow_pppoe_if_num: %u\n"
		"flow_qos_tag: %x (%u)\n"
		"return_qos_tag: %x (%u)\n"
		"flow_dscp: %x\n"
		"return_dscp: %x\n"
		"flow_mirror_ifnum: %u\n"
		"return_mirror_ifnum: %u\n",
		nim,
		nircm->tuple.protocol,
		nircm->conn_rule.flow_mtu,
		nircm->conn_rule.return_mtu,
		&nircm->tuple.flow_ip, nircm->tuple.flow_ident,
		&nircm->tuple.return_ip, nircm->tuple.return_ident,
		&nircm->conn_rule.flow_ip_xlate, nircm->conn_rule.flow_ident_xlate,
		&nircm->conn_rule.return_ip_xlate, nircm->conn_rule.return_ident_xlate,
		nircm->conn_rule.flow_mac,
		nircm->conn_rule.return_mac,
		nircm->conn_rule.flow_interface_num,
		nircm->conn_rule.return_interface_num,
		nircm->vlan_primary_rule.ingress_vlan_tag,
		nircm->vlan_primary_rule.egress_vlan_tag,
		nircm->vlan_secondary_rule.ingress_vlan_tag,
		nircm->vlan_secondary_rule.egress_vlan_tag,
		nircm->rule_flags,
		nircm->valid_flags,
		nircm->pppoe_rule.return_if_exist,
		nircm->pppoe_rule.return_if_num,
		nircm->pppoe_rule.flow_if_exist,
		nircm->pppoe_rule.flow_if_num,
		nircm->qos_rule.flow_qos_tag, nircm->qos_rule.flow_qos_tag,
		nircm->qos_rule.return_qos_tag, nircm->qos_rule.return_qos_tag,
		nircm->dscp_rule.flow_dscp,
		nircm->dscp_rule.return_dscp,
		nircm->mirror_rule.flow_ifnum,
		nircm->mirror_rule.return_ifnum);
}

/*
 * nss_ipv4_log_destroy_rule_msg()
 *	Log IPv4 destroy rule message.
 */
static void nss_ipv4_log_destroy_rule_msg(struct nss_ipv4_msg *nim)
{
	struct nss_ipv4_rule_destroy_msg *nirdm __maybe_unused = &nim->msg.rule_destroy;
	nss_trace("%px: IPv4 destroy rule message: \n"
		"flow_ip: %pI4h:%d\n"
		"return_ip: %pI4h:%d\n"
		"protocol: %d\n",
		nim,
		&nirdm->tuple.flow_ip, nirdm->tuple.flow_ident,
		&nirdm->tuple.return_ip, nirdm->tuple.return_ident,
		nirdm->tuple.protocol);
}

/*
 * nss_ipv4_log_conn_sync()
 *	Log IPv4 connection stats sync message.
 */
static void nss_ipv4_log_conn_sync(struct nss_ipv4_msg *nim)
{
	struct nss_ipv4_conn_sync *sync = &nim->msg.conn_stats;
	if (sync->flow_tx_packet_count || sync->return_tx_packet_count) {
		nss_trace("%px: IPv4 connection stats sync message: \n"
			"Protocol: %d\n"
			"src_addr: %pI4h:%d\n"
			"dest_addr: %pI4h:%d\n"
			"flow_rx_packet_count: %u\n"
			"flow_rx_byte_count: %u\n"
			"return_rx_packet_count: %u\n"
			"return_rx_byte_count: %u\n"
			"flow_tx_packet_count: %u\n"
			"flow_tx_byte_count: %u\n"
			"return_tx_packet_count: %u\n"
			"return_tx_byte_count: %u\n",
			nim,
			(int)sync->protocol,
			&sync->flow_ip, (int)sync->flow_ident,
			&sync->return_ip_xlate, (int)sync->return_ident_xlate,
			sync->flow_rx_packet_count,
			sync->flow_rx_byte_count,
			sync->return_rx_packet_count,
			sync->return_rx_byte_count,
			sync->flow_tx_packet_count,
			sync->flow_tx_byte_count,
			sync->return_tx_packet_count,
			sync->return_tx_byte_count);
		}
}

/*
 * nss_ipv4_log_conn_cfg_msg()
 *	Log IPv4 number of connections supported rule message.
 */
static void nss_ipv4_log_conn_cfg_msg(struct nss_ipv4_msg *nim)
{
	struct nss_ipv4_rule_conn_cfg_msg *nirccm __maybe_unused = &nim->msg.rule_conn_cfg;
	nss_trace("%px: IPv4 number of connections supported rule message: \n"
		"num_conn: %d\n",
		nim,
		nirccm->num_conn);
}

/*
 * nss_ipv4_log_mc_rule_create_msg()
 *	Log IPv4 multicast create rule message.
 */
static void nss_ipv4_log_mc_rule_create_msg(struct nss_ipv4_msg *nim)
{
	uint16_t vif;
	struct nss_ipv4_mc_rule_create_msg *nimrcm = &nim->msg.mc_rule_create;
	for (vif = 0; vif < nimrcm->if_count ; vif++) {
		nss_trace("%px: IPv4 multicast create rule message \n"
			"Rule flag: %x\n"
			"Vif: %d\n"
			"Protocol: %d\n"
			"to_mtu: %u\n"
			"from_ip: %pI4h:%d\n"
			"to_ip: %pI4h:%d\n"
			"to_mac: %pM\n"
			"dest_iface_num: %u\n"
			"out_vlan[0] %x\n"
			"out_vlan[1] %x\n",
			nim,
			nimrcm->if_rule[vif].rule_flags,
			vif,
			nimrcm->tuple.protocol,
			nimrcm->if_rule[vif].if_mtu,
			&nimrcm->tuple.flow_ip, nimrcm->tuple.flow_ident,
			&nimrcm->tuple.return_ip, nimrcm->tuple.return_ident,
			nimrcm->if_rule[vif].if_mac,
			nimrcm->if_rule[vif].if_num,
			nimrcm->if_rule[vif].egress_vlan_tag[0],
			nimrcm->if_rule[vif].egress_vlan_tag[1]);
	}
}

/*
 * nss_ipv4_log_conn_sync_many_msg()
 *	Log IPv4 many conn sync message.
 */
static void nss_ipv4_log_conn_sync_many_msg(struct nss_ipv4_msg *nim)
{
	uint16_t i;
	struct nss_ipv4_conn_sync_many_msg *nicsm = &nim->msg.conn_stats_many;
	for (i = 0; i < nicsm->count; i++) {
		struct nss_ipv4_conn_sync *sync = &nicsm->conn_sync[i];
		if (sync->flow_tx_packet_count || sync->return_tx_packet_count) {
			nss_trace("%px: IPv4 many conn sync message \n"
				"count: %d\n"
				"i: %d\n"
				"Protocol: %d\n"
				"src_addr: %pI4h:%d\n"
				"dest_addr: %pI4h:%d\n"
				"flow_rx_packet_count: %u\n"
				"flow_rx_byte_count: %u\n"
				"return_rx_packet_count: %u\n"
				"return_rx_byte_count: %u\n"
				"flow_tx_packet_count: %u\n"
				"flow_tx_byte_count: %u\n"
				"return_tx_packet_count: %u\n"
				"return_tx_byte_count: %u\n",
				nim,
				nicsm->count,
				i,
				(int)sync->protocol,
				&sync->flow_ip, (int)sync->flow_ident,
				&sync->return_ip_xlate, (int)sync->return_ident_xlate,
				sync->flow_rx_packet_count,
				sync->flow_rx_byte_count,
				sync->return_rx_packet_count,
				sync->return_rx_byte_count,
				sync->flow_tx_packet_count,
				sync->flow_tx_byte_count,
				sync->return_tx_packet_count,
				sync->return_tx_byte_count);
		}
	}
}

/*
 * nss_ipv4_log_verbose()
 *	Log message contents.
 */
static void nss_ipv4_log_verbose(struct nss_ipv4_msg *nim)
{
	switch (nim->cm.type) {
	case NSS_IPV4_TX_CREATE_RULE_MSG:
		nss_ipv4_log_rule_create_msg(nim);
		break;

	case NSS_IPV4_TX_DESTROY_RULE_MSG:
		nss_ipv4_log_destroy_rule_msg(nim);
		break;

	case NSS_IPV4_RX_CONN_STATS_SYNC_MSG:
		nss_ipv4_log_conn_sync(nim);
		break;

	case NSS_IPV4_RX_NODE_STATS_SYNC_MSG:
		/* Getting logged in stats */
		break;

	case NSS_IPV4_TX_CONN_CFG_RULE_MSG:
		nss_ipv4_log_conn_cfg_msg(nim);
		break;

	case NSS_IPV4_TX_CREATE_MC_RULE_MSG:
		nss_ipv4_log_mc_rule_create_msg(nim);
		break;

	case NSS_IPV4_TX_CONN_STATS_SYNC_MANY_MSG:
		nss_ipv4_log_conn_sync_many_msg(nim);
		break;

	default:
		nss_trace("%px: Invalid message type\n", nim);
		break;
	}
}

/*
 * nss_ipv4_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_ipv4_log_tx_msg(struct nss_ipv4_msg *nim)
{
	if (nim->cm.type >= NSS_IPV4_MAX_MSG_TYPES) {
		nss_info("%px: Invalid message type\n", nim);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nim, nim->cm.type, nss_ipv4_log_message_types_str[nim->cm.type]);
	nss_ipv4_log_verbose(nim);
}

/*
 * nss_ipv4_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_ipv4_log_rx_msg(struct nss_ipv4_msg *nim)
{
	if (nim->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_info("%px: Invalid response\n", nim);
		return;
	}

	if (nim->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nim->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nim, nim->cm.type,
			nss_ipv4_log_message_types_str[nim->cm.type],
			nim->cm.response, nss_cmn_response_str[nim->cm.response]);
		goto verbose;
	}

	if (nim->cm.error >= NSS_IPV4_LAST) {
		nss_info("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nim, nim->cm.type, nss_ipv4_log_message_types_str[nim->cm.type],
			nim->cm.response, nss_cmn_response_str[nim->cm.response],
			nim->cm.error);
		goto verbose;
	}

	nss_info("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nim, nim->cm.type, nss_ipv4_log_message_types_str[nim->cm.type],
		nim->cm.response, nss_cmn_response_str[nim->cm.response],
		nim->cm.error, nss_ipv4_log_error_response_types_str[nim->cm.error]);

verbose:
	nss_ipv4_log_verbose(nim);
}
