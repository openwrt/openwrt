/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
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

#ifndef __YPORTENV_H__
#define __YPORTENV_H__

/*
 * Define the MTD version in terms of Linux Kernel versions
 * This allows yaffs to be used independantly of the kernel
 * as well as with it.
 */

#define MTD_VERSION(a, b, c) (((a) << 16) + ((b) << 8) + (c))

#ifdef YAFFS_OUT_OF_TREE
#include "moduleconfig.h"
#endif

#include <linux/version.h>
#define MTD_VERSION_CODE LINUX_VERSION_CODE

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19))
#include <linux/config.h>
#endif
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/xattr.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include <linux/sort.h>
#include <linux/bitops.h>

/*  These type wrappings are used to support Unicode names in WinCE. */
#define YCHAR char
#define YUCHAR unsigned char
#define _Y(x)     x

#define YAFFS_LOSTNFOUND_NAME		"lost+found"
#define YAFFS_LOSTNFOUND_PREFIX		"obj"


#define YAFFS_ROOT_MODE			0755
#define YAFFS_LOSTNFOUND_MODE		0700

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
#define Y_CURRENT_TIME CURRENT_TIME.tv_sec
#define Y_TIME_CONVERT(x) (x).tv_sec
#else
#define Y_CURRENT_TIME CURRENT_TIME
#define Y_TIME_CONVERT(x) (x)
#endif

#define compile_time_assertion(assertion) \
	({ int x = __builtin_choose_expr(assertion, 0, (void)0); (void) x; })


#define yaffs_printf(msk, fmt, ...) \
	printk(KERN_DEBUG "yaffs: " fmt "\n", ##__VA_ARGS__)

#define yaffs_trace(msk, fmt, ...) do { \
	if (yaffs_trace_mask & (msk)) \
		printk(KERN_DEBUG "yaffs: " fmt "\n", ##__VA_ARGS__); \
} while (0)


#endif
