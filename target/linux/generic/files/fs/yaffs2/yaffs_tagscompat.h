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

#ifndef __YAFFS_TAGSCOMPAT_H__
#define __YAFFS_TAGSCOMPAT_H__

#include "yaffs_guts.h"
int yaffs_tags_compat_wr(yaffs_dev_t *dev,
						int nand_chunk,
						const __u8 *data,
						const yaffs_ext_tags *tags);
int yaffs_tags_compat_rd(yaffs_dev_t *dev,
						int nand_chunk,
						__u8 *data,
						yaffs_ext_tags *tags);
int yaffs_tags_compat_mark_bad(struct yaffs_dev_s *dev,
					    int block_no);
int yaffs_tags_compat_query_block(struct yaffs_dev_s *dev,
					  int block_no,
					  yaffs_block_state_t *state,
					  __u32 *seq_number);

void yaffs_calc_tags_ecc(yaffs_tags_t *tags);
int yaffs_check_tags_ecc(yaffs_tags_t *tags);
int yaffs_count_bits(__u8 byte);

#endif
