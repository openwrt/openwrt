/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2018 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __YAFFS_ENDIAN_H__
#define __YAFFS_ENDIAN_H__
#include "yaffs_guts.h"
#include "yaffs_packedtags2.h"

static inline u32 swap_u32(u32 val)
{
	return ((val >>24) & 0x000000ff) |
	       ((val >> 8) & 0x0000ff00) |
	       ((val << 8) & 0x00ff0000) |
	       ((val <<24) & 0xff000000);
}

#define swap_s32(val) \
	(s32)(swap_u32((u32)(val)))

static inline loff_t swap_loff_t(loff_t lval)
{
	u32 vall = swap_u32(FSIZE_LOW(lval));
	u32 valh;

	if (sizeof(loff_t) == sizeof(u32))
		return (loff_t) vall;

	valh = swap_u32(FSIZE_HIGH(lval));

	return FSIZE_COMBINE(vall, valh); /*NB: h and l are swapped. */
}



struct yaffs_dev;
void yaffs_do_endian_s32(struct yaffs_dev *dev, s32 *val);
void yaffs_do_endian_u32(struct yaffs_dev *dev, u32 *val);
void yaffs_do_endian_oh(struct yaffs_dev *dev, struct yaffs_obj_hdr *oh);
void yaffs_do_endian_packed_tags2(struct yaffs_dev *dev,
				struct yaffs_packed_tags2_tags_only *ptt);
void yaffs_endian_config(struct yaffs_dev *dev);

#endif
