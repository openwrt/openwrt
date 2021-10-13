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
#include <nss_l2tpv2.h>
#include "nss_strings.h"

/*
 * nss_l2tpv2_strings_session_stats
 *	L2TPv2 statistics strings for NSS session statistics.
 */
struct nss_stats_info nss_l2tpv2_strings_session_stats[NSS_L2TPV2_STATS_SESSION_MAX] = {
	{"rx_ppp_lcp_pkts"		, NSS_STATS_TYPE_EXCEPTION},
	{"rx_exp_pkts"			, NSS_STATS_TYPE_EXCEPTION},
	{"encap_pbuf_alloc_fails"	, NSS_STATS_TYPE_SPECIAL},
	{"decap_pbuf_alloc_fails"	, NSS_STATS_TYPE_SPECIAL},
	{"decap_l2tpoipsec_src_err"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_l2tpv2_strings_read()
 *	Read L2TPv2 node statistics names.
 */
static ssize_t nss_l2tpv2_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_l2tpv2_strings_session_stats, NSS_L2TPV2_STATS_SESSION_MAX);
}

/*
 * nss_l2tpv2_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(l2tpv2);

/*
 * nss_l2tpv2_strings_dentry_create()
 *	Create L2TPv2 statistics strings debug entry.
 */
void nss_l2tpv2_strings_dentry_create(void)
{
	nss_strings_create_dentry("l2tpv2", &nss_l2tpv2_strings_ops);
}
