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
 * nss_drv_strings_stats
 *	Host driver stats names.
 */
struct nss_stats_info nss_drv_strings_stats[NSS_DRV_STATS_MAX] = {
	{"nbuf_alloc_errors"		, NSS_STATS_TYPE_ERROR},
	{"paged_buf_alloc_errors"	, NSS_STATS_TYPE_ERROR},
	{"tx_queue_full[0]"		, NSS_STATS_TYPE_ERROR},
	{"tx_queue_full[1]"		, NSS_STATS_TYPE_ERROR},
	{"tx_buffers_empty"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_paged_buffers_empty"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffer_pkt"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_cmd"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_crypto"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_reuse"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_buffers_empty"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_buffers_pkt"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_buffers_ext_pkt"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_buffers_cmd_resp"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_buffers_status_sync"	, NSS_STATS_TYPE_SPECIAL},
	{"rx_buffers_crypto"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_buffers_virtual"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_skb_simple"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_skb_nr_frags"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_skb_fraglist"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_skb_simple"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_skb_nr_frags"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_skb_fraglist"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_bad_desciptor"		, NSS_STATS_TYPE_ERROR},
	{"invalid_interface"            , NSS_STATS_TYPE_ERROR},
	{"invalid_core_id"              , NSS_STATS_TYPE_ERROR},
	{"invalid_buffer_type"          , NSS_STATS_TYPE_ERROR},
	{"nss_skb_count"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_chain_seg_processed"	, NSS_STATS_TYPE_SPECIAL},
	{"rx_frag_seg_processed"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_cmd_queue_full"	, NSS_STATS_TYPE_ERROR},
#ifdef NSS_MULTI_H2N_DATA_RING_SUPPORT
	{"tx_buffers_data_queue[0]"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_data_queue[1]"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_data_queue[2]"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_data_queue[3]"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_data_queue[4]"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_data_queue[5]"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_data_queue[6]"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_buffers_data_queue[7]"	, NSS_STATS_TYPE_SPECIAL},
#endif
};

/*
 * nss_drv_strings_read()
 *	Read drv node statistics names.
 */
static ssize_t nss_drv_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_drv_strings_stats, NSS_DRV_STATS_MAX);
}

/*
 * nss_drv_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(drv);

/*
 * nss_drv_strings_dentry_create()
 *      Create drv statistics strings debug entry.
 */
void nss_drv_strings_dentry_create(void)
{
	nss_strings_create_dentry("drv", &nss_drv_strings_ops);
}
