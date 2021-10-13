/*
 ******************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
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
 ******************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"
#include "nss_tls_strings.h"

/*
 * nss_tls_strings_stats
 *	tls statistics strings.
 */
struct nss_stats_info nss_tls_strings_stats[NSS_TLS_STATS_MAX] = {
	{"rx_pkts",			NSS_STATS_TYPE_COMMON},
	{"rx_byts",			NSS_STATS_TYPE_COMMON},
	{"tx_pkts",			NSS_STATS_TYPE_COMMON},
	{"tx_byts",			NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops",		NSS_STATS_TYPE_DROP},
	{"single_rec",			NSS_STATS_TYPE_SPECIAL},
	{"multi_rec",			NSS_STATS_TYPE_SPECIAL},
	{"tx_inval_reqs",		NSS_STATS_TYPE_SPECIAL},
	{"rx_ccs_rec",			NSS_STATS_TYPE_SPECIAL},
	{"fail_ccs",			NSS_STATS_TYPE_ERROR},
	{"eth_node_deactive",		NSS_STATS_TYPE_SPECIAL},
	{"crypto_alloc_success",	NSS_STATS_TYPE_SPECIAL},
	{"crypto_free_req",		NSS_STATS_TYPE_SPECIAL},
	{"crypto_free_success",		NSS_STATS_TYPE_SPECIAL},
	{"fail_crypto_alloc",		NSS_STATS_TYPE_EXCEPTION},
	{"fail_crypto_lookup",		NSS_STATS_TYPE_EXCEPTION},
	{"fail_req_alloc",		NSS_STATS_TYPE_ERROR},
	{"fail_pbuf_stats",		NSS_STATS_TYPE_ERROR},
	{"fail_ctx_active",		NSS_STATS_TYPE_ERROR},
	{"hw_len_error",		NSS_STATS_TYPE_ERROR},
	{"hw_token_error",		NSS_STATS_TYPE_ERROR},
	{"hw_bypass_error",		NSS_STATS_TYPE_ERROR},
	{"hw_crypto_error",		NSS_STATS_TYPE_ERROR},
	{"hw_hash_error",		NSS_STATS_TYPE_ERROR},
	{"hw_config_error",		NSS_STATS_TYPE_ERROR},
	{"hw_algo_error",		NSS_STATS_TYPE_ERROR},
	{"hw_hash_ovf_error",		NSS_STATS_TYPE_ERROR},
	{"hw_auth_error",		NSS_STATS_TYPE_ERROR},
	{"hw_pad_verify_error",		NSS_STATS_TYPE_ERROR},
	{"hw_timeout_error",		NSS_STATS_TYPE_ERROR},
	{"no_desc_in",			NSS_STATS_TYPE_EXCEPTION},
	{"no_desc_out",			NSS_STATS_TYPE_EXCEPTION},
	{"no_reqs",			NSS_STATS_TYPE_EXCEPTION}
};

/*
 * nss_tls_strings_read()
 *	Read tls statistics names
 */
static ssize_t nss_tls_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_tls_strings_stats, NSS_TLS_STATS_MAX);
}

/*
 * nss_tls_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(tls);

/*
 * nss_tls_strings_dentry_create()
 *	Create tls statistics strings debug entry.
 */
void nss_tls_strings_dentry_create(void)
{
	nss_strings_create_dentry("tls", &nss_tls_strings_ops);
}
