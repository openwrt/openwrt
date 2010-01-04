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

void ag71xx_debugfs_exit(struct ag71xx *ag)
{
	debugfs_remove(ag->debug.debugfs_dir);
}

int ag71xx_debugfs_init(struct ag71xx *ag)
{
	ag->debug.debugfs_dir = debugfs_create_dir(ag->dev->name,
						   ag71xx_debugfs_root);
	if (!ag->debug.debugfs_dir)
		goto err;

	return 0;

 err:
	ag71xx_debugfs_exit(ag);
	return -ENOMEM;
}
