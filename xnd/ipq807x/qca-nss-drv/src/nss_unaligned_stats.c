/*
 **************************************************************************
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
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

#include "nss_tx_rx_common.h"
#include "nss_unaligned_stats.h"

/*
 * nss_unaligned_stats_read()
 *	Read unaligned stats
 */
static ssize_t nss_unaligned_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	uint32_t max_output_lines = NSS_MAX_CORES * NSS_UNALIGNED_OPS_PER_MSG;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct nss_unaligned_stats *stats_shadow;
	uint32_t i, j;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_MAX_CORES * sizeof(struct nss_unaligned_stats), GFP_KERNEL);
	if (unlikely(!stats_shadow)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; i < NSS_MAX_CORES; i++) {
		stats_shadow[i] = nss_top_main.nss[i].unaligned_stats;
	}
	spin_unlock_bh(&nss_top_main.stats_lock);

	for (i = 0; i < NSS_MAX_CORES; i++) {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
			"core: %u, total unaligned traps: %llu\n",
			i, stats_shadow[i].trap_count);
		for (j = 0; j < NSS_UNALIGNED_OPS_PER_MSG; j++) {
			struct nss_unaligned_stats_op op = stats_shadow[i].ops[j];
			if (op.count == 0) {
				break;
			}

			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
				"op: %2x, ext: %2x, count:%10llu, min: %10u, avg: %10u, max: %10u\n",
				op.opcode_primary, op.opcode_extension, op.count, op.ticks_min,
				op.ticks_avg, op.ticks_max);
		}
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_unaligned_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(unaligned)

/*
 * nss_unaligned_stats_dentry_create()
 *	Create unaligned statistics debug entry.
 */
void nss_unaligned_stats_dentry_create(void)
{
	nss_stats_create_dentry("unaligned", &nss_unaligned_stats_ops);
}
