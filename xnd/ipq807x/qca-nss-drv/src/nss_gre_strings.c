/*
 ***************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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
 ***************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"
#include "nss_gre_strings.h"

/*
 * nss_gre_strings_base_stats
 *	GRE debug statistics strings for base types
 */
struct nss_stats_info nss_gre_strings_base_stats[NSS_GRE_BASE_DEBUG_MAX] = {
	{"base_rx_pkts",			NSS_STATS_TYPE_COMMON},
	{"base_rx_drops",			NSS_STATS_TYPE_DROP},
	{"base_exp_eth_hdr_missing",		NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_eth_type_non_ip",		NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ip_unknown_protocol",	NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ip_header_incomplete",	NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ip_bad_total_length",	NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ip_bad_checksum",		NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ip_datagram_incomplete",	NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ip_fragment",		NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ip_options_incomplete",	NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ip_with_options",		NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ipv6_unknown_protocol",	NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_ipv6_header_incomplete",	NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_unknown_session",		NSS_STATS_TYPE_EXCEPTION},
	{"base_exp_node_inactive",		NSS_STATS_TYPE_EXCEPTION}
};

/*
 * nss_gre_base_strings_read()
 *	 Read GRE base debug statistics names
 */
static ssize_t nss_gre_base_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_gre_strings_base_stats, NSS_GRE_BASE_DEBUG_MAX);
}

/*
 * nss_gre_base_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(gre_base);

/*
 * nss_gre_strings_session_stats
 *	GRE debug statistics strings for sessions
 */
struct nss_stats_info nss_gre_strings_session_stats[NSS_GRE_SESSION_DEBUG_MAX] = {
	{"session_pbuf_alloc_fail",		NSS_STATS_TYPE_ERROR},
	{"session_decap_forward_enqueue_fail",	NSS_STATS_TYPE_DROP},
	{"session_encap_forward_enqueue_fail",	NSS_STATS_TYPE_DROP},
	{"session_decap_tx_forwarded",		NSS_STATS_TYPE_SPECIAL},
	{"session_encap_rx_received",		NSS_STATS_TYPE_SPECIAL},
	{"session_encap_rx_drops",		NSS_STATS_TYPE_DROP},
	{"session_encap_rx_linear_fail",	NSS_STATS_TYPE_DROP},
	{"session_exp_rx_key_error",		NSS_STATS_TYPE_EXCEPTION},
	{"session_exp_rx_seq_error",		NSS_STATS_TYPE_EXCEPTION},
	{"session_exp_rx_cs_error",		NSS_STATS_TYPE_EXCEPTION},
	{"session_exp_rx_flag_mismatch",	NSS_STATS_TYPE_EXCEPTION},
	{"session_exp_rx_malformed",		NSS_STATS_TYPE_EXCEPTION},
	{"session_exp_rx_invalid_protocol",	NSS_STATS_TYPE_EXCEPTION},
	{"session_exp_rx_no_headroom",		NSS_STATS_TYPE_EXCEPTION}
};

/*
 * nss_gre_session_strings_read()
 *	Read GRE session debug statistics names
 */
static ssize_t nss_gre_session_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_gre_strings_session_stats, NSS_GRE_SESSION_DEBUG_MAX);
}

/*
 * nss_gre_session_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(gre_session);

/*
 * nss_gre_strings_dentry_create()
 *	Create gre statistics strings debug entry.
 */
void nss_gre_strings_dentry_create(void)
{
	struct dentry *gre_d = NULL;

	if (!nss_top_main.strings_dentry) {
		nss_warning("qca-nss-drv/strings is not present");
		return;
	}

	gre_d = debugfs_create_dir("gre", nss_top_main.strings_dentry);
	if (!gre_d) {
		nss_warning("Failed to create qca-nss-drv/strings/gre directory");
		return;
	}

	if (!debugfs_create_file("gre_base", 0400, gre_d, &nss_top_main, &nss_gre_base_strings_ops)) {
		nss_warning("Failed to create qca-nss-drv/strings/gre/gre_base file");
		debugfs_remove_recursive(gre_d);
		return;
	}

	if (!debugfs_create_file("gre_session", 0400, gre_d, &nss_top_main, &nss_gre_session_strings_ops)) {
		nss_warning("Failed to create qca-nss-drv/strings/gre/gre_session file");
		debugfs_remove_recursive(gre_d);
		return;
	}
}
