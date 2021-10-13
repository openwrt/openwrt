/*
 **************************************************************************
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
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

#include "nss_stats.h"
#include "nss_core.h"
#include <nss_ipv6.h>
#include "nss_strings.h"

/*
 * nss_ipv6_strings_exception_stats
 *	Interface statistics strings for IPv6 exceptions.
 */
struct nss_stats_info nss_ipv6_strings_exception_stats[NSS_IPV6_EXCEPTION_EVENT_MAX] = {
	{"icmp_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"icmp_unhandled_type"				, NSS_STATS_TYPE_EXCEPTION},
	{"icmp_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"icmp_udp_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"icmp_tcp_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"icmp_unknown_protocol"			, NSS_STATS_TYPE_EXCEPTION},
	{"icmp_no_icme"					, NSS_STATS_TYPE_EXCEPTION},
	{"icmp_flush_to_host"				, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_no_icme"					, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_small_hop_limit"				, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_needs_fragmentation"			, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_flags"					, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_seq_exceeds_right_edge"			, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_small_data_offs"				, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_bad_sack"					, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_big_data_offs"				, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_seq_before_left_edge"			, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_ack_exceeds_right_edge"			, NSS_STATS_TYPE_EXCEPTION},
	{"tcp_ack_before_left_edge"			, NSS_STATS_TYPE_EXCEPTION},
	{"udp_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"udp_no_icme"					, NSS_STATS_TYPE_EXCEPTION},
	{"udp_small_hop_limit"				, NSS_STATS_TYPE_EXCEPTION},
	{"udp_needs_fragmentation"			, NSS_STATS_TYPE_EXCEPTION},
	{"wrong_target_mac"				, NSS_STATS_TYPE_EXCEPTION},
	{"header_incomplete"				, NSS_STATS_TYPE_EXCEPTION},
	{"unknown_protocol"				, NSS_STATS_TYPE_EXCEPTION},
	{"ingress_vid_mismatch"				, NSS_STATS_TYPE_EXCEPTION},
	{"ingress_vid_missing"				, NSS_STATS_TYPE_EXCEPTION},
	{"dscp_marking_mismatch"			, NSS_STATS_TYPE_EXCEPTION},
	{"vlan_marking_mismatch"			, NSS_STATS_TYPE_EXCEPTION},
	{"interface_mismatch"				, NSS_STATS_TYPE_EXCEPTION},
	{"gre_no_icme"					, NSS_STATS_TYPE_EXCEPTION},
	{"gre_needs_fragmentation"			, NSS_STATS_TYPE_EXCEPTION},
	{"gre_small_hop_limit"				, NSS_STATS_TYPE_EXCEPTION},
	{"destroy"					, NSS_STATS_TYPE_EXCEPTION},
	{"icmp_udplite_header_incomplete"		, NSS_STATS_TYPE_EXCEPTION},
	{"udplite_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"udplite_no_icme"				, NSS_STATS_TYPE_EXCEPTION},
	{"udplite_small_hop_limit"			, NSS_STATS_TYPE_EXCEPTION},
	{"udplite_needs_fragmentation"			, NSS_STATS_TYPE_EXCEPTION},
	{"mc_udp_no_icme"				, NSS_STATS_TYPE_EXCEPTION},
	{"mc_mem_alloc_failure"				, NSS_STATS_TYPE_EXCEPTION},
	{"mc_update_failure"				, NSS_STATS_TYPE_EXCEPTION},
	{"mc_pbuf_alloc_failure"			, NSS_STATS_TYPE_EXCEPTION},
	{"esp_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"esp_no_icme"					, NSS_STATS_TYPE_EXCEPTION},
	{"esp_ip_fragment"				, NSS_STATS_TYPE_EXCEPTION},
	{"esp_small_hop_limit"				, NSS_STATS_TYPE_EXCEPTION},
	{"esp_needs_fragmentation"			, NSS_STATS_TYPE_EXCEPTION},
	{"tunipip6_no_icme"				, NSS_STATS_TYPE_EXCEPTION},
	{"tunipip6_small_hop_limit"			, NSS_STATS_TYPE_EXCEPTION},
	{"tunipip6_needs_fragmentation"			, NSS_STATS_TYPE_EXCEPTION},
	{"pppoe_bridge_no_icme"				, NSS_STATS_TYPE_EXCEPTION},
	{"dont_frag_set"				, NSS_STATS_TYPE_EXCEPTION},
	{"reassembly_not_supported"			, NSS_STATS_TYPE_EXCEPTION},
	{"pppoe_no_session"				, NSS_STATS_TYPE_DROP},
	{"icmp_gre_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"icmp_esp_header_incomplete"			, NSS_STATS_TYPE_EXCEPTION},
	{"emesh_prio_mismatch"				, NSS_STATS_TYPE_EXCEPTION},
};

/*
 * nss_ipv6_strings_stats
 *	IPv6 stats strings.
 */
struct nss_stats_info nss_ipv6_strings_stats[NSS_IPV6_STATS_MAX] = {
	{"rx_pkts"			,NSS_STATS_TYPE_SPECIAL},
	{"rx_bytes"			,NSS_STATS_TYPE_SPECIAL},
	{"tx_pkts"			,NSS_STATS_TYPE_SPECIAL},
	{"tx_bytes"			,NSS_STATS_TYPE_SPECIAL},
	{"create_requests"		,NSS_STATS_TYPE_SPECIAL},
	{"create_collisions"		,NSS_STATS_TYPE_SPECIAL},
	{"create_invalid_interface"	,NSS_STATS_TYPE_SPECIAL},
	{"destroy_requests"		,NSS_STATS_TYPE_SPECIAL},
	{"destroy_misses"		,NSS_STATS_TYPE_SPECIAL},
	{"hash_hits"			,NSS_STATS_TYPE_SPECIAL},
	{"hash_reorders"		,NSS_STATS_TYPE_SPECIAL},
	{"flushes"			,NSS_STATS_TYPE_SPECIAL},
	{"evictions"			,NSS_STATS_TYPE_SPECIAL},
	{"fragmentations"		,NSS_STATS_TYPE_SPECIAL},
	{"frag_fails"			,NSS_STATS_TYPE_SPECIAL},
	{"by_rule_drops"		,NSS_STATS_TYPE_DROP},
	{"mc_create_requests"		,NSS_STATS_TYPE_SPECIAL},
	{"mc_update_requests"		,NSS_STATS_TYPE_SPECIAL},
	{"mc_create_invalid_interface"	,NSS_STATS_TYPE_SPECIAL},
	{"mc_destroy_requests"		,NSS_STATS_TYPE_SPECIAL},
	{"mc_destroy_misses"		,NSS_STATS_TYPE_SPECIAL},
	{"mc_flushes"			,NSS_STATS_TYPE_SPECIAL},
	{"mirror_invalid_ifnum_conn_create_req"		,NSS_STATS_TYPE_SPECIAL},
	{"mirror_invalid_iftype_conn_create_req"	,NSS_STATS_TYPE_SPECIAL},
	{"mirror_failures"		,NSS_STATS_TYPE_SPECIAL},
};

/*
 * nss_ipv6_special_stats_strings_read()
 *	Read IPv6 special node statistics names.
 */
static ssize_t nss_ipv6_special_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_ipv6_strings_stats, NSS_IPV6_STATS_MAX);
}

/*
 * nss_ipv6_exception_stats_strings_read()
 *	Read IPv6 exception statistics names.
 */
static ssize_t nss_ipv6_exception_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_ipv6_strings_exception_stats, NSS_IPV6_EXCEPTION_EVENT_MAX);
}

/*
 * nss_ipv6_special_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(ipv6_special_stats);

/*
 * nss_ipv6_exception_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(ipv6_exception_stats);

/*
 * nss_ipv6_strings_dentry_create()
 *      Create IPv6 statistics strings debug entry.
 */
void nss_ipv6_strings_dentry_create(void)
{
	struct dentry *ipv6_d = NULL;
	struct dentry *ipv6_spcl_stats_d = NULL;
	struct dentry *ipv6_excp_stats_d = NULL;

	if (!nss_top_main.strings_dentry) {
		nss_warning("qca-nss-drv/strings is not present");
		return;
	}

	ipv6_d = debugfs_create_dir("ipv6", nss_top_main.strings_dentry);
	if (!ipv6_d) {
		nss_warning("Failed to create qca-nss-drv/strings/ipv6 directory");
		return;
	}

	ipv6_spcl_stats_d = debugfs_create_file("special_stats_str", 0400, ipv6_d, &nss_top_main, &nss_ipv6_special_stats_strings_ops);
	if (!ipv6_spcl_stats_d) {
		nss_warning("Failed to create qca-nss-drv/stats/ipv6/special_stats_str file");
		debugfs_remove_recursive(ipv6_d);
		return;
	}

	ipv6_excp_stats_d = debugfs_create_file("exception_stats_str", 0400, ipv6_d, &nss_top_main, &nss_ipv6_exception_stats_strings_ops);
	if (!ipv6_excp_stats_d) {
		nss_warning("Failed to create qca-nss-drv/stats/ipv6/exception_stats_str file");
		debugfs_remove_recursive(ipv6_d);
		return;
	}
}
