/*
 **************************************************************************
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
 **************************************************************************
 */

#include "nss_core.h"
#include "nss_crypto_cmn_stats.h"
#include "nss_crypto_cmn_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_crypto_cmn_stats_notifier);

/*
 * Spinlock to protect CRYPTO_CMN statistics update/read
 */
DEFINE_SPINLOCK(nss_crypto_cmn_stats_lock);

/*
 * nss_crypto_cmn_stats
 *	crypto common statistics
 */
uint64_t nss_crypto_cmn_stats[NSS_CRYPTO_CMN_STATS_MAX];

/*
 * nss_crypto_cmn_stats_read()
 *	Read crypto common statistics
 */
static ssize_t nss_crypto_cmn_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;

	/*
	 * Max output lines = #stats +
	 * few blank lines for banner printing + Number of Extra outputlines
	 * for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_CRYPTO_CMN_STATS_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;

	char *lbuf = vzalloc(size_al);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return -ENOMEM;
	}

	stats_shadow = vzalloc(NSS_CRYPTO_CMN_STATS_MAX * 8);
	if (unlikely(!stats_shadow)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		vfree(lbuf);
		return -ENOMEM;
	}

	/*
	 * crypto common statistics
	 */
	spin_lock_bh(&nss_crypto_cmn_stats_lock);
	for (i = 0; i < NSS_CRYPTO_CMN_STATS_MAX; i++)
		stats_shadow[i] = nss_crypto_cmn_stats[i];

	spin_unlock_bh(&nss_crypto_cmn_stats_lock);
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "crypto_cmn", NSS_STATS_SINGLE_CORE);
	size_wr += nss_stats_print("crypto_cmn", NULL, NSS_STATS_SINGLE_INSTANCE, nss_crypto_cmn_strings_stats,
					stats_shadow, NSS_CRYPTO_CMN_STATS_MAX, lbuf, size_wr, size_al);

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	vfree(lbuf);
	vfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_crypto_cmn_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(crypto_cmn);

/*
 * nss_crypto_cmn_stats_dentry_create()
 *	Create crypto common statistics debug entry.
 */
void nss_crypto_cmn_stats_dentry_create(void)
{
	nss_stats_create_dentry("crypto_cmn", &nss_crypto_cmn_stats_ops);
}

/*
 * nss_crypto_cmn_stats_sync()
 *	Handle the syncing of NSS crypto common statistics.
 */
void nss_crypto_cmn_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_crypto_cmn_stats *nct)
{
	int j;

	spin_lock_bh(&nss_crypto_cmn_stats_lock);

	/*
	 * Common node stats
	 */
	nss_crypto_cmn_stats[NSS_STATS_NODE_RX_PKTS] += nct->nstats.rx_packets;
	nss_crypto_cmn_stats[NSS_STATS_NODE_RX_BYTES] += nct->nstats.rx_bytes;
	nss_crypto_cmn_stats[NSS_STATS_NODE_TX_PKTS] += nct->nstats.tx_packets;
	nss_crypto_cmn_stats[NSS_STATS_NODE_TX_BYTES] += nct->nstats.tx_bytes;

	for (j = 0; j < NSS_MAX_NUM_PRI; j++)
		nss_crypto_cmn_stats[NSS_STATS_NODE_RX_QUEUE_0_DROPPED + j] += nct->nstats.rx_dropped[j];

	/*
	 * crypto common statistics
	 */
	nss_crypto_cmn_stats[NSS_CRYPTO_CMN_STATS_FAIL_VERSION] += nct->fail_version;
	nss_crypto_cmn_stats[NSS_CRYPTO_CMN_STATS_FAIL_CTX] += nct->fail_ctx;
	nss_crypto_cmn_stats[NSS_CRYPTO_CMN_STATS_FAIL_DMA] += nct->fail_dma;

	spin_unlock_bh(&nss_crypto_cmn_stats_lock);
}

/*
 * nss_crypto_cmn_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_crypto_cmn_stats_notify(struct nss_ctx_instance *nss_ctx)
{
	struct nss_crypto_cmn_stats_notification crypto_cmn_stats;

	crypto_cmn_stats.core_id = nss_ctx->id;
	memcpy(crypto_cmn_stats.stats, nss_crypto_cmn_stats, sizeof(crypto_cmn_stats.stats));
	atomic_notifier_call_chain(&nss_crypto_cmn_stats_notifier, NSS_STATS_EVENT_NOTIFY, &crypto_cmn_stats);
}

/*
 * nss_crypto_cmn_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_crypto_cmn_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_crypto_cmn_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_crypto_cmn_stats_register_notifier);

/*
 * nss_crypto_cmn_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_crypto_cmn_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_crypto_cmn_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_crypto_cmn_stats_unregister_notifier);
