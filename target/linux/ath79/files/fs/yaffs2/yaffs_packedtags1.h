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

/* This is used to pack YAFFS1 tags, not YAFFS2 tags. */

#ifndef __YAFFS_PACKEDTAGS1_H__
#define __YAFFS_PACKEDTAGS1_H__

#include "yaffs_guts.h"

struct yaffs_packed_tags1 {
	u32 chunk_id:20;
	u32 serial_number:2;
	u32 n_bytes:10;
	u32 obj_id:18;
	u32 ecc:12;
	u32 deleted:1;
	u32 unused_stuff:1;
	unsigned should_be_ff;

};

void yaffs_pack_tags1(struct yaffs_packed_tags1 *pt,
		      const struct yaffs_ext_tags *t);
void yaffs_unpack_tags1(struct yaffs_ext_tags *t,
			const struct yaffs_packed_tags1 *pt);
#endif
