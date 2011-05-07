/*
 *  Atheros AR71xx built-in ethernet mac driver
 *
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Based on Atheros' AG7100 driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/debugfs.h>

#include "ag71xx.h"

static struct dentry *ag71xx_debugfs_root;

static int ag71xx_debugfs_generic_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

void ag71xx_debugfs_update_int_stats(struct ag71xx *ag, u32 status)
{
	if (status)
		ag->debug.int_stats.total++;
	if (status & AG71XX_INT_TX_PS)
		ag->debug.int_stats.tx_ps++;
	if (status & AG71XX_INT_TX_UR)
		ag->debug.int_stats.tx_ur++;
	if (status & AG71XX_INT_TX_BE)
		ag->debug.int_stats.tx_be++;
	if (status & AG71XX_INT_RX_PR)
		ag->debug.int_stats.rx_pr++;
	if (status & AG71XX_INT_RX_OF)
		ag->debug.int_stats.rx_of++;
	if (status & AG71XX_INT_RX_BE)
		ag->debug.int_stats.rx_be++;
}

static ssize_t read_file_int_stats(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
#define PR_INT_STAT(_label, _field)					\
	len += snprintf(buf + len, sizeof(buf) - len,			\
		"%20s: %10lu\n", _label, ag->debug.int_stats._field);

	struct ag71xx *ag = file->private_data;
	char buf[256];
	unsigned int len = 0;

	PR_INT_STAT("TX Packet Sent", tx_ps);
	PR_INT_STAT("TX Underrun", tx_ur);
	PR_INT_STAT("TX Bus Error", tx_be);
	PR_INT_STAT("RX Packet Received", rx_pr);
	PR_INT_STAT("RX Overflow", rx_of);
	PR_INT_STAT("RX Bus Error", rx_be);
	len += snprintf(buf + len, sizeof(buf) - len, "\n");
	PR_INT_STAT("Total", total);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
#undef PR_INT_STAT
}

static const struct file_operations ag71xx_fops_int_stats = {
	.open	= ag71xx_debugfs_generic_open,
	.read	= read_file_int_stats,
	.owner	= THIS_MODULE
};

void ag71xx_debugfs_update_napi_stats(struct ag71xx *ag, int rx, int tx)
{
	struct ag71xx_napi_stats *stats = &ag->debug.napi_stats;

	if (rx) {
		stats->rx_count++;
		stats->rx_packets += rx;
		if (rx <= AG71XX_NAPI_WEIGHT)
			stats->rx[rx]++;
		if (rx > stats->rx_packets_max)
			stats->rx_packets_max = rx;
	}

	if (tx) {
		stats->tx_count++;
		stats->tx_packets += tx;
		if (tx <= AG71XX_NAPI_WEIGHT)
			stats->tx[tx]++;
		if (tx > stats->tx_packets_max)
			stats->tx_packets_max = tx;
	}
}

static ssize_t read_file_napi_stats(struct file *file, char __user *user_buf,
				    size_t count, loff_t *ppos)
{
	struct ag71xx *ag = file->private_data;
	struct ag71xx_napi_stats *stats = &ag->debug.napi_stats;
	char buf[2048];
	unsigned int len = 0;
	unsigned long rx_avg = 0;
	unsigned long tx_avg = 0;
	int i;

	if (stats->rx_count)
		rx_avg = stats->rx_packets / stats->rx_count;

	if (stats->tx_count)
		tx_avg = stats->tx_packets / stats->tx_count;

	len += snprintf(buf + len, sizeof(buf) - len, "%3s  %10s %10s\n",
			"len", "rx", "tx");

	for (i = 1; i <= AG71XX_NAPI_WEIGHT; i++)
		len += snprintf(buf + len, sizeof(buf) - len,
				"%3d: %10lu %10lu\n",
				i, stats->rx[i], stats->tx[i]);

	len += snprintf(buf + len, sizeof(buf) - len, "\n");

	len += snprintf(buf + len, sizeof(buf) - len, "%3s: %10lu %10lu\n",
			"sum", stats->rx_count, stats->tx_count);
	len += snprintf(buf + len, sizeof(buf) - len, "%3s: %10lu %10lu\n",
			"avg", rx_avg, tx_avg);
	len += snprintf(buf + len, sizeof(buf) - len, "%3s: %10lu %10lu\n",
			"max", stats->rx_packets_max, stats->tx_packets_max);
	len += snprintf(buf + len, sizeof(buf) - len, "%3s: %10lu %10lu\n",
			"pkt", stats->rx_packets, stats->tx_packets);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static const struct file_operations ag71xx_fops_napi_stats = {
	.open	= ag71xx_debugfs_generic_open,
	.read	= read_file_napi_stats,
	.owner	= THIS_MODULE
};

void ag71xx_debugfs_exit(struct ag71xx *ag)
{
	debugfs_remove_recursive(ag->debug.debugfs_dir);
}

int ag71xx_debugfs_init(struct ag71xx *ag)
{
	ag->debug.debugfs_dir = debugfs_create_dir(ag->dev->name,
						   ag71xx_debugfs_root);
	if (!ag->debug.debugfs_dir)
		return -ENOMEM;

	debugfs_create_file("int_stats", S_IRUGO, ag->debug.debugfs_dir,
			    ag, &ag71xx_fops_int_stats);
	debugfs_create_file("napi_stats", S_IRUGO, ag->debug.debugfs_dir,
			    ag, &ag71xx_fops_napi_stats);

	return 0;
}

int ag71xx_debugfs_root_init(void)
{
	if (ag71xx_debugfs_root)
		return -EBUSY;

	ag71xx_debugfs_root = debugfs_create_dir(KBUILD_MODNAME, NULL);
	if (!ag71xx_debugfs_root)
		return -ENOENT;

	return 0;
}

void ag71xx_debugfs_root_exit(void)
{
	debugfs_remove(ag71xx_debugfs_root);
	ag71xx_debugfs_root = NULL;
}
