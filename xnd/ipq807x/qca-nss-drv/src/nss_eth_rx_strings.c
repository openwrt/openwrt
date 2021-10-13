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

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"

/*
 * nss_eth_rx_strings_stats
 *	Ethernet Rx statistics strings.
 */
struct nss_stats_info nss_eth_rx_strings_stats[NSS_ETH_RX_STATS_MAX] = {
	{"ticks"	, NSS_STATS_TYPE_SPECIAL},
	{"worst_ticks"	, NSS_STATS_TYPE_SPECIAL},
	{"iterations"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_eth_rx_strings_exception_stats
 *	Interface statistics strings for unknown exceptions.
 */
struct nss_stats_info nss_eth_rx_strings_exception_stats[NSS_ETH_RX_EXCEPTION_EVENT_MAX] = {
	{"unknown_l3_protocol"	, NSS_STATS_TYPE_EXCEPTION},
	{"eth_hdr_missing"	, NSS_STATS_TYPE_EXCEPTION},
	{"vlan_missing"		, NSS_STATS_TYPE_EXCEPTION},
	{"trustsec_hdr_missing"	, NSS_STATS_TYPE_EXCEPTION}
};

/*
 * nss_eth_rx_special_stats_strings_read()
 *	Read Ethernet Rx special node statistics names.
 */
static ssize_t nss_eth_rx_special_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_eth_rx_strings_stats, NSS_ETH_RX_STATS_MAX);
}

/*
 * nss_eth_rx_exception_stats_strings_read()
 *	Read Ethernet Rx exception statistics names.
 */
static ssize_t nss_eth_rx_exception_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_eth_rx_strings_exception_stats, NSS_ETH_RX_EXCEPTION_EVENT_MAX);
}

/*
 * nss_eth_rx_special_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(eth_rx_special_stats);

/*
 * nss_eth_rx_exception_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(eth_rx_exception_stats);

/*
 * nss_eth_rx_strings_dentry_create()
 *      Create Ethernet Rx statistics strings debug entry.
 */
void nss_eth_rx_strings_dentry_create(void)
{
	struct dentry *eth_rx_d = NULL;
	struct dentry *eth_rx_spcl_stats_d = NULL;
	struct dentry *eth_rx_excp_stats_d = NULL;

	if (!nss_top_main.strings_dentry) {
		nss_warning("qca-nss-drv/strings is not present");
		return;
	}

	eth_rx_d = debugfs_create_dir("eth_rx", nss_top_main.strings_dentry);
	if (!eth_rx_d) {
		nss_warning("Failed to create qca-nss-drv/strings/eth_rx directory");
		return;
	}

	eth_rx_spcl_stats_d = debugfs_create_file("special_stats_str", 0400, eth_rx_d, &nss_top_main, &nss_eth_rx_special_stats_strings_ops);
	if (!eth_rx_spcl_stats_d) {
		nss_warning("Failed to create qca-nss-drv/stats/eth_rx/special_stats_str file");
		debugfs_remove_recursive(eth_rx_d);
		return;
	}

	eth_rx_excp_stats_d = debugfs_create_file("exception_stats_str", 0400, eth_rx_d, &nss_top_main, &nss_eth_rx_exception_stats_strings_ops);
	if (!eth_rx_excp_stats_d) {
		nss_warning("Failed to create qca-nss-drv/stats/eth_rx/exception_stats_str file");
		debugfs_remove_recursive(eth_rx_d);
		return;
	}
}
