/*
 **************************************************************************
 * Copyright (c) 2017, 2019, The Linux Foundation. All rights reserved.
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
 **************************************************************************
 */

#include "nss_core.h"
#include "nss_dtls_stats.h"

/*
 * nss_dtls_stats_session_str
 *	DTLS statistics strings for nss session stats.
 */
struct nss_stats_info nss_dtls_stats_session_str[NSS_DTLS_STATS_SESSION_MAX] = {
	{"rx_pkts"			, NSS_STATS_TYPE_COMMON},
	{"tx_pkts"			, NSS_STATS_TYPE_COMMON},
	{"rx_drops[0]"			, NSS_STATS_TYPE_DROP},
	{"rx_drops[1]"			, NSS_STATS_TYPE_DROP},
	{"rx_drops[2]"			, NSS_STATS_TYPE_DROP},
	{"rx_drops[3]"			, NSS_STATS_TYPE_DROP},
	{"rx_auth_done"			, NSS_STATS_TYPE_SPECIAL},
	{"tx_auth_done"			, NSS_STATS_TYPE_SPECIAL},
	{"rx_cipher_done"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_cipher_done"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_cbuf_alloc_fail"		, NSS_STATS_TYPE_DROP},
	{"tx_cbuf_alloc_fail"		, NSS_STATS_TYPE_DROP},
	{"tx_cenqueue_fail"		, NSS_STATS_TYPE_DROP},
	{"rx_cenqueue_fail"		, NSS_STATS_TYPE_DROP},
	{"tx_drops_hroom"		, NSS_STATS_TYPE_DROP},
	{"tx_drops_troom"		, NSS_STATS_TYPE_DROP},
	{"tx_forward_enqueue_fail"	, NSS_STATS_TYPE_DROP},
	{"rx_forward_enqueue_fail"	, NSS_STATS_TYPE_DROP},
	{"rx_invalid_version"		, NSS_STATS_TYPE_DROP},
	{"rx_invalid_epoch"		, NSS_STATS_TYPE_DROP},
	{"rx_malformed"			, NSS_STATS_TYPE_DROP},
	{"rx_cipher_fail"		, NSS_STATS_TYPE_EXCEPTION},
	{"rx_auth_fail"			, NSS_STATS_TYPE_EXCEPTION},
	{"rx_capwap_classify_fail"	, NSS_STATS_TYPE_DROP},
	{"rx_single_rec_dgram"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_multi_rec_dgram"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_replay_fail"		, NSS_STATS_TYPE_DROP},
	{"rx_replay_duplicate"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_replay_out_of_window"	, NSS_STATS_TYPE_SPECIAL},
	{"outflow_queue_full"		, NSS_STATS_TYPE_DROP},
	{"decap_queue_full"		, NSS_STATS_TYPE_DROP},
	{"pbuf_alloc_fail"		, NSS_STATS_TYPE_DROP},
	{"pbuf_copy_fail"		, NSS_STATS_TYPE_DROP},
	{"epoch"			, NSS_STATS_TYPE_DROP},
	{"tx_seq_high"			, NSS_STATS_TYPE_SPECIAL},
	{"tx_seq_low"			, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_dtls_stats_read()
 *	Read DTLS session statistics.
 */
static ssize_t nss_dtls_stats_read(struct file *fp, char __user *ubuf,
				   size_t sz, loff_t *ppos)
{
	uint32_t max_output_lines = 2 + (NSS_MAX_DTLS_SESSIONS
					* (NSS_DTLS_STATS_SESSION_MAX + 2)) + 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	int id;
	struct nss_dtls_stats_session *dtls_session_stats = NULL;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	dtls_session_stats = kzalloc((sizeof(struct nss_dtls_stats_session)
				     * NSS_MAX_DTLS_SESSIONS), GFP_KERNEL);
	if (unlikely(dtls_session_stats == NULL)) {
		nss_warning("Could not allocate memory for populating DTLS stats");
		kfree(lbuf);
		return 0;
	}

	/*
	 * Get all stats.
	 */
	nss_dtls_session_stats_get(dtls_session_stats);

	/*
	 * Session stats.
	 */
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "dtls", NSS_STATS_SINGLE_CORE);

	for (id = 0; id < NSS_MAX_DTLS_SESSIONS; id++) {
		if (!dtls_session_stats[id].valid)
			break;

		dev = dev_get_by_index(&init_net, dtls_session_stats[id].if_index);
		if (likely(dev)) {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
					     "%d. nss interface id=%d, netdevice=%s\n",
					     id, dtls_session_stats[id].if_num,
					     dev->name);
			dev_put(dev);
		} else {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
					     "%d. nss interface id=%d\n", id,
					     dtls_session_stats[id].if_num);
		}

		size_wr += nss_stats_print("dtls_cmn", NULL, id, nss_dtls_stats_session_str, dtls_session_stats[id].stats, NSS_DTLS_STATS_SESSION_MAX, lbuf, size_wr, size_al);
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(dtls_session_stats);
	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_dtls_stats_ops.
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(dtls)

/*
 * nss_dtls_stats_dentry_create()
 *	Create DTLS statistics debug entry.
 */
void nss_dtls_stats_dentry_create(void)
{
	nss_stats_create_dentry("dtls", &nss_dtls_stats_ops);
}
