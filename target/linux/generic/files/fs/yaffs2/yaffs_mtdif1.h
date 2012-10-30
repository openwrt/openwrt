/*
 * YAFFS: Yet another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __YAFFS_MTDIF1_H__
#define __YAFFS_MTDIF1_H__

int nandmtd1_WriteChunkWithTagsToNAND(yaffs_dev_t *dev, int nand_chunk,
	const __u8 *data, const yaffs_ext_tags *tags);

int nandmtd1_ReadChunkWithTagsFromNAND(yaffs_dev_t *dev, int nand_chunk,
	__u8 *data, yaffs_ext_tags *tags);

int nandmtd1_MarkNANDBlockBad(struct yaffs_dev_s *dev, int block_no);

int nandmtd1_QueryNANDBlock(struct yaffs_dev_s *dev, int block_no,
	yaffs_block_state_t *state, __u32 *seq_number);

#endif
