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

/* This is used to pack YAFFS1 tags, not YAFFS2 tags. */

#ifndef __YAFFS_PACKEDTAGS1_H__
#define __YAFFS_PACKEDTAGS1_H__

#include "yaffs_guts.h"

typedef struct {
	unsigned chunk_id:20;
	unsigned serial_number:2;
	unsigned n_bytes:10;
	unsigned obj_id:18;
	unsigned ecc:12;
	unsigned deleted:1;
	unsigned unusedStuff:1;
	unsigned shouldBeFF;

} yaffs_PackedTags1;

void yaffs_PackTags1(yaffs_PackedTags1 *pt, const yaffs_ext_tags *t);
void yaffs_unpack_tags1(yaffs_ext_tags *t, const yaffs_PackedTags1 *pt);
#endif
