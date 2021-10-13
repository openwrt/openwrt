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
#include "nss_dma_stats.h"

/*
 * nss_dma_strings_stats
 *	DMA statistics strings.
 */
struct nss_stats_info nss_dma_strings_stats[NSS_DMA_STATS_MAX] = {
	{"rx_pkts"		, NSS_STATS_TYPE_COMMON},
	{"rx_byts"		, NSS_STATS_TYPE_COMMON},
	{"tx_pkts"		, NSS_STATS_TYPE_COMMON},
	{"tx_byts"		, NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops"	, NSS_STATS_TYPE_DROP},
	{"no_req"		, NSS_STATS_TYPE_SPECIAL},
	{"no_desc"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_nexthop"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_nexthop_queue"	, NSS_STATS_TYPE_SPECIAL},
	{"fail_linear_sz"	, NSS_STATS_TYPE_SPECIAL},
	{"fail_linear_alloc"	, NSS_STATS_TYPE_SPECIAL},
	{"fail_linear_no_sg"	, NSS_STATS_TYPE_SPECIAL},
	{"fail_split_sz"	, NSS_STATS_TYPE_SPECIAL},
	{"fail_split_alloc"	, NSS_STATS_TYPE_SPECIAL},
	{"fail_sync_alloc"	, NSS_STATS_TYPE_SPECIAL},
	{"fail_ctx_active"	, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[0]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[1]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[2]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[3]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[4]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[5]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[6]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[7]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[8]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[9]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[10]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[11]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[12]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[13]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[14]"		, NSS_STATS_TYPE_SPECIAL},
	{"fail_hw[15]"		, NSS_STATS_TYPE_SPECIAL},
};


/*
 * nss_dma_strings_read()
 *	Read DMA node statistics names
 */
static ssize_t nss_dma_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_dma_strings_stats, NSS_DMA_STATS_MAX);
}

/*
 * nss_dma_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(dma);

/*
 * nss_dma_strings_dentry_create()
 *	Create DMA statistics strings debug entry.
 */
void nss_dma_strings_dentry_create(void)
{
	nss_strings_create_dentry("dma", &nss_dma_strings_ops);
}
