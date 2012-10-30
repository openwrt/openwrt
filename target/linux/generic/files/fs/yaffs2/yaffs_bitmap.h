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

/*
 * Chunk bitmap manipulations
 */

#ifndef __YAFFS_BITMAP_H__
#define __YAFFS_BITMAP_H__

#include "yaffs_guts.h"

void yaffs_verify_chunk_bit_id(yaffs_dev_t *dev, int blk, int chunk);
void yaffs_clear_chunk_bits(yaffs_dev_t *dev, int blk);
void yaffs_clear_chunk_bit(yaffs_dev_t *dev, int blk, int chunk);
void yaffs_set_chunk_bit(yaffs_dev_t *dev, int blk, int chunk);
int yaffs_check_chunk_bit(yaffs_dev_t *dev, int blk, int chunk);
int yaffs_still_some_chunks(yaffs_dev_t *dev, int blk);
int yaffs_count_chunk_bits(yaffs_dev_t *dev, int blk);

#endif
