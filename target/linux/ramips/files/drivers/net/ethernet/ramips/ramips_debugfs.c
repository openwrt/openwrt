/*
 *  Ralink SoC ethernet driver debugfs code
 *
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/phy.h>

#include "ramips_eth.h"

static struct dentry *raeth_debugfs_root;

static int raeth_debugfs_generic_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

void raeth_debugfs_update_int_stats(struct raeth_priv *re, u32 status)
{
	re->debug.int_stats.total += !!status;

	re->debug.int_stats.rx_delayed += !!(status & RAMIPS_RX_DLY_INT);
	re->debug.int_stats.rx_done0 += !!(status & RAMIPS_RX_DONE_INT0);
	re->debug.int_stats.rx_coherent += !!(status & RAMIPS_RX_COHERENT);

	re->debug.int_stats.tx_delayed += !!(status & RAMIPS_TX_DLY_INT);
	re->debug.int_stats.tx_done0 += !!(status & RAMIPS_TX_DONE_INT0);
	re->debug.int_stats.tx_done1 += !!(status & RAMIPS_TX_DONE_INT1);
	re->debug.int_stats.tx_done2 += !!(status & RAMIPS_TX_DONE_INT2);
	re->debug.int_stats.tx_done3 += !!(status & RAMIPS_TX_DONE_INT3);
	re->debug.int_stats.tx_coherent += !!(status & RAMIPS_TX_COHERENT);

	re->debug.int_stats.pse_fq_empty += !!(status & RAMIPS_PSE_FQ_EMPTY);
	re->debug.int_stats.pse_p0_fc += !!(status & RAMIPS_PSE_P0_FC);
	re->debug.int_stats.pse_p1_fc += !!(status & RAMIPS_PSE_P1_FC);
	re->debug.int_stats.pse_p2_fc += !!(status & RAMIPS_PSE_P2_FC);
	re->debug.int_stats.pse_buf_drop += !!(status & RAMIPS_PSE_BUF_DROP);
}

static ssize_t read_file_int_stats(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
#define PR_INT_STAT(_label, _field)					\
	len += snprintf(buf + len, sizeof(buf) - len,			\
		"%-18s: %10lu\n", _label, re->debug.int_stats._field);

	struct raeth_priv *re = file->private_data;
	char buf[512];
	unsigned int len = 0;
	unsigned long flags;

	spin_lock_irqsave(&re->page_lock, flags);

	PR_INT_STAT("RX Delayed", rx_delayed);
	PR_INT_STAT("RX Done 0", rx_done0);
	PR_INT_STAT("RX Coherent", rx_coherent);

	PR_INT_STAT("TX Delayed", tx_delayed);
	PR_INT_STAT("TX Done 0", tx_done0);
	PR_INT_STAT("TX Done 1", tx_done1);
	PR_INT_STAT("TX Done 2", tx_done2);
	PR_INT_STAT("TX Done 3", tx_done3);
	PR_INT_STAT("TX Coherent", tx_coherent);

	PR_INT_STAT("PSE FQ empty", pse_fq_empty);
	PR_INT_STAT("CDMA Flow control", pse_p0_fc);
	PR_INT_STAT("GDMA1 Flow control", pse_p1_fc);
	PR_INT_STAT("GDMA2 Flow control", pse_p2_fc);
	PR_INT_STAT("PSE discard", pse_buf_drop);

	len += snprintf(buf + len, sizeof(buf) - len, "\n");
	PR_INT_STAT("Total", total);

	spin_unlock_irqrestore(&re->page_lock, flags);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
#undef PR_INT_STAT
}

static const struct file_operations raeth_fops_int_stats = {
	.open	= raeth_debugfs_generic_open,
	.read	= read_file_int_stats,
	.owner	= THIS_MODULE
};

void raeth_debugfs_exit(struct raeth_priv *re)
{
	debugfs_remove_recursive(re->debug.debugfs_dir);
}

int raeth_debugfs_init(struct raeth_priv *re)
{
	re->debug.debugfs_dir = debugfs_create_dir(re->netdev->name,
						   raeth_debugfs_root);
	if (!re->debug.debugfs_dir)
		return -ENOMEM;

	debugfs_create_file("int_stats", S_IRUGO, re->debug.debugfs_dir,
			    re, &raeth_fops_int_stats);

	return 0;
}

int raeth_debugfs_root_init(void)
{
	if (raeth_debugfs_root)
		return -EBUSY;

	raeth_debugfs_root = debugfs_create_dir("raeth", NULL);
	if (!raeth_debugfs_root)
		return -ENOENT;

	return 0;
}

void raeth_debugfs_root_exit(void)
{
	debugfs_remove(raeth_debugfs_root);
	raeth_debugfs_root = NULL;
}
