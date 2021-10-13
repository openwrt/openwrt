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
#include <nss_capwap.h>
#include "nss_strings.h"
#include "nss_capwap_strings.h"

/*
 * nss_capwap_strings_encap_stats
 *	CAPWAP encap statistics string.
 */
struct nss_stats_info nss_capwap_strings_encap_stats[NSS_CAPWAP_STATS_ENCAP_MAX] = {
	{"tx_packets",		NSS_STATS_TYPE_COMMON},
	{"tx_bytes",		NSS_STATS_TYPE_COMMON},
	{"tx_segments",		NSS_STATS_TYPE_SPECIAL},
	{"tx_drop_seg_ref",	NSS_STATS_TYPE_DROP},
	{"tx_drop_ver_mismatch",NSS_STATS_TYPE_DROP},
	{"tx_dropped_inner",	NSS_STATS_TYPE_DROP},
	{"tx_drop_hroom",	NSS_STATS_TYPE_DROP},
	{"tx_drop_DTLS",	NSS_STATS_TYPE_DROP},
	{"tx_drop_nwireless",	NSS_STATS_TYPE_DROP},
	{"tx_drop_qfull",	NSS_STATS_TYPE_DROP},
	{"tx_drop_mem_fail",	NSS_STATS_TYPE_DROP},
	{"fast_mem",		NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_capwap_encap_strings_read()
 *	Read CAPWAP encap statistics names.
 */
static ssize_t nss_capwap_encap_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_capwap_strings_encap_stats, NSS_CAPWAP_STATS_ENCAP_MAX);
}

/*
 * nss_capwap_encap_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(capwap_encap);

/*
 * nss_capwap_strings_decap_stats
 *	CAPWAP decap statistics string.
 */
struct nss_stats_info nss_capwap_strings_decap_stats[NSS_CAPWAP_STATS_DECAP_MAX] = {
	{"rx_packets",		NSS_STATS_TYPE_COMMON},
	{"rx_bytes",		NSS_STATS_TYPE_COMMON},
	{"rx_DTLS_pkts",	NSS_STATS_TYPE_SPECIAL},
	{"rx_segments",		NSS_STATS_TYPE_SPECIAL},
	{"rx_dropped",		NSS_STATS_TYPE_DROP},
	{"rx_drop_oversize",	NSS_STATS_TYPE_DROP},
	{"rx_drop_frag_timeout",NSS_STATS_TYPE_DROP},
	{"rx_drop_frag_dup",	NSS_STATS_TYPE_DROP},
	{"rx_drop_frag_gap",	NSS_STATS_TYPE_DROP},
	{"rx_drop_n2h",		NSS_STATS_TYPE_DROP},
	{"rx_drop_n2h_qfull",	NSS_STATS_TYPE_DROP},
	{"rx_drop_mem_fail",	NSS_STATS_TYPE_DROP},
	{"rx_drop_csum",	NSS_STATS_TYPE_DROP},
	{"rx_drop_malformed",	NSS_STATS_TYPE_DROP},
	{"fast_mem",		NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_capwap_decap_strings_read()
 *	Read CAPWAP decap statistics names.
 */
static ssize_t nss_capwap_decap_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_capwap_strings_decap_stats, NSS_CAPWAP_STATS_DECAP_MAX);
}

/*
 * nss_capwap_decap_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(capwap_decap);

/*
 * nss_capwap_strings_dentry_create()
 *	Create CAPWAP statistics strings debug entry.
 */
void nss_capwap_strings_dentry_create(void)
{
	nss_strings_create_dentry("capwap_encap", &nss_capwap_encap_strings_ops);
	nss_strings_create_dentry("capwap_decap", &nss_capwap_decap_strings_ops);
}
