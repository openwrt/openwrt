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

#ifndef __YAFFS_NAND_H__
#define __YAFFS_NAND_H__
#include "yaffs_guts.h"



int yaffs_rd_chunk_tags_nand(yaffs_dev_t *dev, int nand_chunk,
					__u8 *buffer,
					yaffs_ext_tags *tags);

int yaffs_wr_chunk_tags_nand(yaffs_dev_t *dev,
						int nand_chunk,
						const __u8 *buffer,
						yaffs_ext_tags *tags);

int yaffs_mark_bad(yaffs_dev_t *dev, int block_no);

int yaffs_query_init_block_state(yaffs_dev_t *dev,
						int block_no,
						yaffs_block_state_t *state,
						unsigned *seq_number);

int yaffs_erase_block(struct yaffs_dev_s *dev,
				  int flash_block);

int yaffs_init_nand(struct yaffs_dev_s *dev);

#endif

