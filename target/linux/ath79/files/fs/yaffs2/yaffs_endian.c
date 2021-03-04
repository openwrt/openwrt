/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2018 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Endian processing functions.
 */

#include "yaffs_endian.h"
#include "yaffs_guts.h"


void yaffs_do_endian_u32(struct yaffs_dev *dev, u32 *val)
{
	if (!dev->swap_endian)
		return;
	*val = swap_u32(*val);
}

void yaffs_do_endian_s32(struct yaffs_dev *dev, s32 *val)
{
	if (!dev->swap_endian)
		return;
	*val = swap_s32(*val);
}

void yaffs_do_endian_oh(struct yaffs_dev *dev, struct yaffs_obj_hdr *oh)
{
	if (!dev->swap_endian)
		return;
	/* Change every field */
	oh->type = swap_u32(oh->type);
	oh->parent_obj_id = swap_u32(oh->parent_obj_id);

	oh->yst_mode = swap_u32(oh->yst_mode);

	oh->yst_uid = swap_u32(oh->yst_uid);
	oh->yst_gid = swap_u32(oh->yst_gid);
	oh->yst_atime = swap_u32(oh->yst_atime);
	oh->yst_mtime = swap_u32(oh->yst_mtime);
	oh->yst_ctime = swap_u32(oh->yst_ctime);

	oh->file_size_low = swap_u32(oh->file_size_low);

	oh->equiv_id = swap_u32(oh->equiv_id);

	oh->yst_rdev = swap_u32(oh->yst_rdev);

	oh->win_ctime[0] = swap_u32(oh->win_ctime[0]);
	oh->win_ctime[1] = swap_u32(oh->win_ctime[1]);
	oh->win_atime[0] = swap_u32(oh->win_atime[0]);
	oh->win_atime[1] = swap_u32(oh->win_atime[1]);
	oh->win_mtime[0] = swap_u32(oh->win_mtime[0]);
	oh->win_mtime[1] = swap_u32(oh->win_mtime[1]);

	oh->inband_shadowed_obj_id = swap_u32(oh->inband_shadowed_obj_id);
	oh->inband_is_shrink = swap_u32(oh->inband_is_shrink);

	oh->file_size_high = swap_u32(oh->file_size_high);
	oh->reserved[0] = swap_u32(oh->reserved[0]);
	oh->shadows_obj = swap_s32(oh->shadows_obj);

	oh->is_shrink = swap_u32(oh->is_shrink);
}


void yaffs_do_endian_packed_tags2(struct yaffs_dev *dev,
				struct yaffs_packed_tags2_tags_only *ptt)
{
	if (!dev->swap_endian)
		return;
	ptt->seq_number = swap_u32(ptt->seq_number);
	ptt->obj_id = swap_u32(ptt->obj_id);
	ptt->chunk_id = swap_u32(ptt->chunk_id);
	ptt->n_bytes = swap_u32(ptt->n_bytes);
}

void yaffs_endian_config(struct yaffs_dev *dev)
{
	u32 x = 1;

	if (dev->tnode_size < 1)
		BUG();

	dev->swap_endian = 0;

	if (((char *)&x)[0] == 1) {
		/* Little Endian. */
		if (dev->param.stored_endian == 2 /* big endian */)
			dev->swap_endian = 1;
	} else  {
		/* Big Endian. */
		if (dev->param.stored_endian == 1 /* little endian */)
			dev->swap_endian = 1;
	}

	if (dev->swap_endian)
		dev->tn_swap_buffer = kmalloc(dev->tnode_size, GFP_NOFS);
}
