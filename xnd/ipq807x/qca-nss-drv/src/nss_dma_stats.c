/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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

#include "nss_core.h"
#include "nss_dma.h"
#include "nss_dma_stats.h"
#include "nss_dma_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_dma_stats_notifier);

/*
 * Spinlock to protect DMA statistics update/read
 */
DEFINE_SPINLOCK(nss_dma_stats_lock);

/*
 * nss_dma_stats
 *	DMA statistics
 */
uint64_t nss_dma_stats[NSS_DMA_STATS_MAX];

/*
 * nss_dma_stats_read()
 *	Read DMA statistics
 */
static ssize_t nss_dma_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * Max output lines = #stats * NSS_MAX_CORES  +
	 * few blank lines for banner printing + Number of Extra outputlines for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_DMA_STATS_MAX  + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	uint64_t *stats_shadow;
	ssize_t bytes_read = 0;
	size_t size_wr = 0;
	char *lbuf;
	int32_t i;

	lbuf = vzalloc(size_al);
	if (!lbuf) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return -ENOMEM;
	}

	stats_shadow = vzalloc(NSS_DMA_STATS_MAX * 8);
	if (!stats_shadow) {
		nss_warning("Could not allocate memory for local shadow buffer");
		vfree(lbuf);
		return -ENOMEM;
	}

	/*
	 * DMA statistics
	 */
	spin_lock_bh(&nss_dma_stats_lock);
	for (i = 0; i < NSS_DMA_STATS_MAX; i++) {
		stats_shadow[i] = nss_dma_stats[i];
	}
	spin_unlock_bh(&nss_dma_stats_lock);

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "dma", nss_top_main.dma_handler_id);
	size_wr += nss_stats_print("dma", NULL, NSS_STATS_SINGLE_INSTANCE, nss_dma_strings_stats,
				stats_shadow, NSS_DMA_STATS_MAX, lbuf, size_wr, size_al);

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));

	vfree(lbuf);
	vfree(stats_shadow);
	return bytes_read;
}

/*
 * nss_dma_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(dma);

/*
 * nss_dma_stats_dentry_create()
 *	Create DMA statistics debug entry.
 */
void nss_dma_stats_dentry_create(void)
{
	nss_stats_create_dentry("dma", &nss_dma_stats_ops);
}

/*
 * nss_dma_stats_sync()
 *	Handle the syncing of NSS DMA statistics.
 */
void nss_dma_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_dma_stats *nds)
{
	uint64_t *dma_stats;
	uint32_t *msg_stats;
	uint16_t i = 0;

	spin_lock_bh(&nss_dma_stats_lock);

	msg_stats = (uint32_t *)nds;
	dma_stats = nss_dma_stats;

	for (i = 0; i < NSS_DMA_STATS_MAX; i++, dma_stats++, msg_stats++) {
		*dma_stats += *msg_stats;
	}

	spin_unlock_bh(&nss_dma_stats_lock);
}

/*
 * nss_dma_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_dma_stats_notify(struct nss_ctx_instance *nss_ctx)
{
	struct nss_dma_stats_notification dma_stats;

	spin_lock_bh(&nss_dma_stats_lock);
	dma_stats.core_id = nss_ctx->id;
	memcpy(dma_stats.stats_ctx, nss_dma_stats, sizeof(dma_stats.stats_ctx));
	spin_unlock_bh(&nss_dma_stats_lock);

	atomic_notifier_call_chain(&nss_dma_stats_notifier, NSS_STATS_EVENT_NOTIFY, &dma_stats);
}

/*
 * nss_dma_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_dma_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_dma_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_dma_stats_unregister_notifier);

/*
 * nss_dma_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_dma_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_dma_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_dma_stats_register_notifier);
