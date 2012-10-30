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

/*
 * This file is just holds extra declarations of macros that would normally
 * be providesd in the Linux kernel. These macros have been written from
 * scratch but are functionally equivalent to the Linux ones.
 *
 */

#ifndef __EXTRAS_H__
#define __EXTRAS_H__


#include "yportenv.h"

#if !(defined __KERNEL__)

/* Definition of types */
typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned __u32;

#endif


#if !(defined __KERNEL__)


#ifndef WIN32
#include <sys/stat.h>
#endif


#ifdef CONFIG_YAFFS_PROVIDE_DEFS
/* File types */


#define DT_UNKNOWN	0
#define DT_FIFO		1
#define DT_CHR		2
#define DT_DIR		4
#define DT_BLK		6
#define DT_REG		8
#define DT_LNK		10
#define DT_SOCK		12
#define DT_WHT		14


#ifndef WIN32
#include <sys/stat.h>
#endif

/*
 * Attribute flags.  These should be or-ed together to figure out what
 * has been changed!
 */
#define ATTR_MODE	1
#define ATTR_UID	2
#define ATTR_GID	4
#define ATTR_SIZE	8
#define ATTR_ATIME	16
#define ATTR_MTIME	32
#define ATTR_CTIME	64

struct iattr {
	unsigned int ia_valid;
	unsigned ia_mode;
	unsigned ia_uid;
	unsigned ia_gid;
	unsigned ia_size;
	unsigned ia_atime;
	unsigned ia_mtime;
	unsigned ia_ctime;
	unsigned int ia_attr_flags;
};

#endif

#else

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/stat.h>

#endif


#endif
