/*
 ****************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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
 ****************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"
#include "nss_dtls_cmn_strings.h"

/*
 * nss_dtls_cmn_ctx_stats_str
 *	 dtls common ctx statistics strings.
 */
struct nss_stats_info nss_dtls_cmn_ctx_stats_str[NSS_DTLS_CMN_CTX_STATS_MAX] = {
	{"rx_pkts",			NSS_STATS_TYPE_COMMON},
	{"rx_byts",			NSS_STATS_TYPE_COMMON},
	{"tx_pkts",			NSS_STATS_TYPE_COMMON},
	{"tx_byts",			NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_single_rec",		NSS_STATS_TYPE_SPECIAL},
	{"rx_multi_rec",		NSS_STATS_TYPE_SPECIAL},
	{"fail_crypto_resource",	NSS_STATS_TYPE_DROP},
	{"fail_crypto_enqueue",		NSS_STATS_TYPE_DROP},
	{"fail_headroom",		NSS_STATS_TYPE_DROP},
	{"fail_tailroom",		NSS_STATS_TYPE_DROP},
	{"fail_ver",			NSS_STATS_TYPE_DROP},
	{"fail_epoch",			NSS_STATS_TYPE_DROP},
	{"fail_dtls_record",		NSS_STATS_TYPE_DROP},
	{"fail_capwap",			NSS_STATS_TYPE_DROP},
	{"fail_replay",			NSS_STATS_TYPE_DROP},
	{"fail_replay_dup",		NSS_STATS_TYPE_DROP},
	{"fail_replay_win",		NSS_STATS_TYPE_DROP},
	{"fail_queue",			NSS_STATS_TYPE_DROP},
	{"fail_queue_nexthop",		NSS_STATS_TYPE_DROP},
	{"fail_pbuf_alloc",		NSS_STATS_TYPE_DROP},
	{"fail_pbuf_linear",		NSS_STATS_TYPE_DROP},
	{"fail_pbuf_stats",		NSS_STATS_TYPE_DROP},
	{"fail_pbuf_align",		NSS_STATS_TYPE_DROP},
	{"fail_ctx_active",		NSS_STATS_TYPE_DROP},
	{"fail_hwctx_active",		NSS_STATS_TYPE_DROP},
	{"fail_cipher",			NSS_STATS_TYPE_EXCEPTION},
	{"fail_auth",			NSS_STATS_TYPE_EXCEPTION},
	{"fail_seq_ovf",		NSS_STATS_TYPE_DROP},
	{"fail_blk_len",		NSS_STATS_TYPE_DROP},
	{"fail_hash_len",		NSS_STATS_TYPE_DROP},
	{"len_error",			NSS_STATS_TYPE_DROP},
	{"token_error",			NSS_STATS_TYPE_DROP},
	{"bypass_error",		NSS_STATS_TYPE_DROP},
	{"config_error",		NSS_STATS_TYPE_DROP},
	{"algo_error",			NSS_STATS_TYPE_DROP},
	{"hash_ovf_error",		NSS_STATS_TYPE_DROP},
	{"ttl_error",			NSS_STATS_TYPE_DROP},
	{"csum_error",			NSS_STATS_TYPE_DROP},
	{"timeout_error",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[0]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[1]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[2]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[3]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[4]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[5]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[6]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[7]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[8]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[9]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[10]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[11]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[12]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[13]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[14]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[15]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[16]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[17]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[18]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[19]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[20]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[21]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[22]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[23]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[24]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[25]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[26]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[27]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[28]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[29]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[30]",		NSS_STATS_TYPE_DROP},
	{"fail_cle_[31]",		NSS_STATS_TYPE_DROP},
	{"seq_low",			NSS_STATS_TYPE_SPECIAL},
	{"seq_high",			NSS_STATS_TYPE_SPECIAL},
	{"epoch",			NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_dtls_cmn_ctx_stats_str_strings_read()
 *	Read dtls common ctx statistics names
 */
static ssize_t nss_dtls_cmn_ctx_stats_str_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_dtls_cmn_ctx_stats_str, NSS_DTLS_CMN_CTX_STATS_MAX);
}

/*
 * nss_dtls_cmn_ctx_stats_str_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(dtls_cmn_ctx_stats_str);

/*
 * nss_dtls_cmn_strings_dentry_create()
 *	Create dtls common statistics strings debug entry.
 */
void nss_dtls_cmn_strings_dentry_create(void)
{
	nss_strings_create_dentry("dtls_cmn_ctx_stats_str", &nss_dtls_cmn_ctx_stats_str_strings_ops);
}
