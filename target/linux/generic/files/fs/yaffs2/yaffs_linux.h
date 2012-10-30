/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __YAFFS_LINUX_H__
#define __YAFFS_LINUX_H__

#include "devextras.h"
#include "yportenv.h"

struct yaffs_LinuxContext {
	struct ylist_head	contextList; /* List of these we have mounted */
	struct yaffs_dev_s *dev;
	struct super_block * superBlock;
	struct task_struct *bgThread; /* Background thread for this device */
	int bgRunning;
        struct semaphore grossLock;     /* Gross locking semaphore */
	__u8 *spareBuffer;      /* For mtdif2 use. Don't know the size of the buffer
				 * at compile time so we have to allocate it.
				 */
	struct ylist_head searchContexts;
	void (*putSuperFunc)(struct super_block *sb);

	struct task_struct *readdirProcess;
	unsigned mount_id;
};

#define yaffs_dev_to_lc(dev) ((struct yaffs_LinuxContext *)((dev)->os_context))
#define yaffs_dev_to_mtd(dev) ((struct mtd_info *)((dev)->driver_context))

#endif

