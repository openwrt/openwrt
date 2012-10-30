/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __YAFFS_YAFFS2_H__
#define __YAFFS_YAFFS2_H__

#include "yaffs_guts.h"

void yaffs_calc_oldest_dirty_seq(yaffs_dev_t *dev);
void yaffs2_find_oldest_dirty_seq(yaffs_dev_t *dev);
void yaffs2_clear_oldest_dirty_seq(yaffs_dev_t *dev, yaffs_block_info_t *bi);
void yaffs2_update_oldest_dirty_seq(yaffs_dev_t *dev, unsigned block_no, yaffs_block_info_t *bi);
int yaffs_block_ok_for_gc(yaffs_dev_t *dev, yaffs_block_info_t *bi);
__u32 yaffs2_find_refresh_block(yaffs_dev_t *dev);
int yaffs2_checkpt_required(yaffs_dev_t *dev);
int yaffs_calc_checkpt_blocks_required(yaffs_dev_t *dev);


void yaffs2_checkpt_invalidate(yaffs_dev_t *dev);
int yaffs2_checkpt_save(yaffs_dev_t *dev);
int yaffs2_checkpt_restore(yaffs_dev_t *dev);

int yaffs2_handle_hole(yaffs_obj_t *obj, loff_t new_size);
int yaffs2_scan_backwards(yaffs_dev_t *dev);

#endif
