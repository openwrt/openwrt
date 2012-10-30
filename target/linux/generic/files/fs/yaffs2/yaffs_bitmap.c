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

#include "yaffs_bitmap.h"
#include "yaffs_trace.h"
/*
 * Chunk bitmap manipulations
 */

static Y_INLINE __u8 *yaffs_BlockBits(yaffs_dev_t *dev, int blk)
{
	if (blk < dev->internal_start_block || blk > dev->internal_end_block) {
		T(YAFFS_TRACE_ERROR,
			(TSTR("**>> yaffs: BlockBits block %d is not valid" TENDSTR),
			blk));
		YBUG();
	}
	return dev->chunk_bits +
		(dev->chunk_bit_stride * (blk - dev->internal_start_block));
}

void yaffs_verify_chunk_bit_id(yaffs_dev_t *dev, int blk, int chunk)
{
	if (blk < dev->internal_start_block || blk > dev->internal_end_block ||
			chunk < 0 || chunk >= dev->param.chunks_per_block) {
		T(YAFFS_TRACE_ERROR,
		(TSTR("**>> yaffs: Chunk Id (%d:%d) invalid"TENDSTR),
			blk, chunk));
		YBUG();
	}
}

void yaffs_clear_chunk_bits(yaffs_dev_t *dev, int blk)
{
	__u8 *blkBits = yaffs_BlockBits(dev, blk);

	memset(blkBits, 0, dev->chunk_bit_stride);
}

void yaffs_clear_chunk_bit(yaffs_dev_t *dev, int blk, int chunk)
{
	__u8 *blkBits = yaffs_BlockBits(dev, blk);

	yaffs_verify_chunk_bit_id(dev, blk, chunk);

	blkBits[chunk / 8] &= ~(1 << (chunk & 7));
}

void yaffs_set_chunk_bit(yaffs_dev_t *dev, int blk, int chunk)
{
	__u8 *blkBits = yaffs_BlockBits(dev, blk);

	yaffs_verify_chunk_bit_id(dev, blk, chunk);

	blkBits[chunk / 8] |= (1 << (chunk & 7));
}

int yaffs_check_chunk_bit(yaffs_dev_t *dev, int blk, int chunk)
{
	__u8 *blkBits = yaffs_BlockBits(dev, blk);
	yaffs_verify_chunk_bit_id(dev, blk, chunk);

	return (blkBits[chunk / 8] & (1 << (chunk & 7))) ? 1 : 0;
}

int yaffs_still_some_chunks(yaffs_dev_t *dev, int blk)
{
	__u8 *blkBits = yaffs_BlockBits(dev, blk);
	int i;
	for (i = 0; i < dev->chunk_bit_stride; i++) {
		if (*blkBits)
			return 1;
		blkBits++;
	}
	return 0;
}

int yaffs_count_chunk_bits(yaffs_dev_t *dev, int blk)
{
	__u8 *blkBits = yaffs_BlockBits(dev, blk);
	int i;
	int n = 0;
	for (i = 0; i < dev->chunk_bit_stride; i++) {
		__u8 x = *blkBits;
		while (x) {
			if (x & 1)
				n++;
			x >>= 1;
		}

		blkBits++;
	}
	return n;
}

