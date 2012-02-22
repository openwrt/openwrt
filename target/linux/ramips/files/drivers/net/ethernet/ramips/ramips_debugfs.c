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
#include <linux/phy.h>

#include "ramips_eth.h"

static struct dentry *raeth_debugfs_root;

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
