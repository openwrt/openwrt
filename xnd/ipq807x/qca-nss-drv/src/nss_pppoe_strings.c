/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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
#include <nss_pppoe.h>
#include "nss_strings.h"

/*
 * nss_pppoe_strings_session_stats
 *	PPPoE session stats strings.
 */
struct nss_stats_info nss_pppoe_strings_session_stats[NSS_PPPOE_STATS_SESSION_MAX]  = {
	{"rx_pkts"			, NSS_STATS_TYPE_COMMON},
	{"rx_byts"			, NSS_STATS_TYPE_COMMON},
	{"tx_pkts"			, NSS_STATS_TYPE_COMMON},
	{"tx_byts"			, NSS_STATS_TYPE_COMMON},
	{"wrong_version_or_type"	, NSS_STATS_TYPE_EXCEPTION},
	{"wrong_code"			, NSS_STATS_TYPE_EXCEPTION},
	{"unsupported_ppp_protocol"	, NSS_STATS_TYPE_EXCEPTION}
};

/*
 * nss_pppoe_strings_base_stats
 *	PPPoE base node stats strings.
 */
struct nss_stats_info nss_pppoe_strings_base_stats[NSS_PPPOE_STATS_BASE_MAX] = {
	{"rx_packets"                   , NSS_STATS_TYPE_COMMON},
	{"rx_bytes"                     , NSS_STATS_TYPE_COMMON},
	{"tx_packets"                   , NSS_STATS_TYPE_COMMON},
	{"tx_bytes"                     , NSS_STATS_TYPE_COMMON},
	{"rx_dropped[0]"                , NSS_STATS_TYPE_DROP},
	{"rx_dropped[1]"                , NSS_STATS_TYPE_DROP},
	{"rx_dropped[2]"                , NSS_STATS_TYPE_DROP},
	{"rx_dropped[3]"                , NSS_STATS_TYPE_DROP},
	{"short_pppoe_hdr_length"       , NSS_STATS_TYPE_EXCEPTION},
	{"short_packet_length"          , NSS_STATS_TYPE_EXCEPTION},
	{"wrong_version_or_type"        , NSS_STATS_TYPE_EXCEPTION},
	{"wrong_code"                   , NSS_STATS_TYPE_EXCEPTION},
	{"unsupported_ppp_protocol"     , NSS_STATS_TYPE_EXCEPTION},
	{"disabled_bridge_packet"       , NSS_STATS_TYPE_EXCEPTION}
};

/*
 * nss_pppoe_isession_stats_strings_read()
 *	Read PPPoE session statistics names.
 */
static ssize_t nss_pppoe_session_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_pppoe_strings_session_stats, NSS_PPPOE_STATS_SESSION_MAX);
}

/*
 * nss_pppoe_base_stats_strings_read()
 *      Read PPPoE base statistics names.
 */
static ssize_t nss_pppoe_base_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_pppoe_strings_base_stats, NSS_PPPOE_STATS_BASE_MAX);
}

/*
 * nss_pppoe_session_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(pppoe_session_stats);

/*
 * nss_pppoe_base_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(pppoe_base_stats);

/*
 * nss_pppoe_strings_dentry_create()
 *	Create PPPoE statistics strings debug entry.
 */
void nss_pppoe_strings_dentry_create(void)
{
	struct dentry *pppoe_d = NULL;
	struct dentry *pppoe_session_stats_d = NULL;
	struct dentry *pppoe_base_stats_d = NULL;

	if (!nss_top_main.strings_dentry) {
		nss_warning("qca-nss-drv/strings is not present");
		return;
	}

	pppoe_d = debugfs_create_dir("pppoe", nss_top_main.strings_dentry);
	if (!pppoe_d) {
		nss_warning("Failed to create qca-nss-drv/strings/pppoe directory");
		return;
	}

	pppoe_session_stats_d = debugfs_create_file("session_stats_str", 0400, pppoe_d, &nss_top_main, &nss_pppoe_session_stats_strings_ops);
	if (!pppoe_session_stats_d) {
		nss_warning("Failed to create qca-nss-drv/stats/pppoe/session_stats_str file");
		debugfs_remove_recursive(pppoe_d);
		return;
	}

	pppoe_base_stats_d = debugfs_create_file("base_stats_str", 0400, pppoe_d, &nss_top_main, &nss_pppoe_base_stats_strings_ops);
	if (!pppoe_base_stats_d) {
		nss_warning("Failed to create qca-nss-drv/stats/pppoe/base_stats_str file");
		debugfs_remove_recursive(pppoe_d);
		return;
	}
}
