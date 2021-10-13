/*
 **************************************************************************
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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

/*
 * nss_freq_stats.c
 *	NSS Frequency statistics APIs.
 */

#include "nss_stats.h"
#include "nss_tx_rx_common.h"

/*
 * At any point, this object has the latest data about CPU utilization.
 */
extern struct nss_freq_cpu_usage nss_freq_cpu_status;

/*
 * Spinlock to protect the global data structure nss_freq_cpu_status
 */
extern spinlock_t nss_freq_cpu_usage_lock;

/*
 * nss_freq_stats_read()
 * 	Read frequency stats and display CPU information.
 */
static ssize_t nss_freq_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * max output lines = Should change in case of number of lines below.
	 */
	uint32_t max_output_lines = (2 + 3) + 5;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint32_t avg, max, min;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	size_wr = scnprintf(lbuf, size_al, "CPU Utilization:\n");

	spin_lock_bh(&nss_freq_cpu_usage_lock);
	avg = nss_freq_cpu_status.used;
	max = nss_freq_cpu_status.max;
	min = nss_freq_cpu_status.min;
	spin_unlock_bh(&nss_freq_cpu_usage_lock);

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "Note: Averaged over 1 second\n\n");
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "Core 0:\n");
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "Min\tAvg\tMax\n");
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, " %u%%\t %u%%\t %u%%\n\n", min, avg, max);

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);

	return bytes_read;
}

/*
 * nss_freq_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(freq)

/*
 * nss_freq_dentry_create()
 */
void nss_freq_stats_dentry_create(void)
{
	nss_stats_create_dentry("cpu_load_ubi", &nss_freq_stats_ops);
}