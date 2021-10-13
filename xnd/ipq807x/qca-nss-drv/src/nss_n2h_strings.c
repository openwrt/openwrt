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
#include <nss_n2h.h>
#include "nss_strings.h"

/*
 * nss_n2h_strings_stats
 *	N2H statistics strings.
 */
struct nss_stats_info nss_n2h_strings_stats[NSS_N2H_STATS_MAX] = {
	{"rx_pkts"			, NSS_STATS_TYPE_COMMON},
	{"rx_byts"			, NSS_STATS_TYPE_COMMON},
	{"tx_pkts"			, NSS_STATS_TYPE_COMMON},
	{"tx_byts"			, NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops"		, NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops"		, NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops"		, NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops"		, NSS_STATS_TYPE_DROP},
	{"queue_drops"			, NSS_STATS_TYPE_DROP},
	{"ticks"			, NSS_STATS_TYPE_SPECIAL},
	{"worst_ticks"			, NSS_STATS_TYPE_SPECIAL},
	{"iterations"			, NSS_STATS_TYPE_SPECIAL},
	{"pbuf_ocm_total_count"		, NSS_STATS_TYPE_SPECIAL},
	{"pbuf_ocm_free_count"		, NSS_STATS_TYPE_SPECIAL},
	{"pbuf_ocm_alloc_fail_payload"	, NSS_STATS_TYPE_SPECIAL},
	{"pbuf_ocm_alloc_fail_nopayload", NSS_STATS_TYPE_SPECIAL},
	{"pbuf_def_total_count"		, NSS_STATS_TYPE_SPECIAL},
	{"pbuf_def_free_count"		, NSS_STATS_TYPE_SPECIAL},
	{"pbuf_def_alloc_fail_payload"	, NSS_STATS_TYPE_SPECIAL},
	{"pbuf_def_alloc_fail_nopayload", NSS_STATS_TYPE_SPECIAL},
	{"payload_alloc_fails"		, NSS_STATS_TYPE_SPECIAL},
	{"payload_free_count"		, NSS_STATS_TYPE_SPECIAL},
	{"h2n_control_pkts"		, NSS_STATS_TYPE_SPECIAL},
	{"h2n_control_byts"		, NSS_STATS_TYPE_SPECIAL},
	{"n2h_control_pkts"		, NSS_STATS_TYPE_SPECIAL},
	{"n2h_control_byts"		, NSS_STATS_TYPE_SPECIAL},
	{"h2n_data_pkts"		, NSS_STATS_TYPE_SPECIAL},
	{"h2n_data_byts"		, NSS_STATS_TYPE_SPECIAL},
	{"n2h_data_pkts"		, NSS_STATS_TYPE_SPECIAL},
	{"n2h_data_byts"		, NSS_STATS_TYPE_SPECIAL},
	{"n2h_tot_payloads"		, NSS_STATS_TYPE_SPECIAL},
	{"n2h_data_interface_invalid"	, NSS_STATS_TYPE_SPECIAL},
	{"n2h_enqueue_retries"		, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_n2h_strings_read()
 *	Read N2H node statistics names.
 */
static ssize_t nss_n2h_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_n2h_strings_stats, NSS_N2H_STATS_MAX);
}

/*
 * nss_n2h_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(n2h);

/*
 * nss_n2h_strings_dentry_create()
 *	Create N2H statistics strings debug entry.
 */
void nss_n2h_strings_dentry_create(void)
{
	nss_strings_create_dentry("n2h", &nss_n2h_strings_ops);
}
