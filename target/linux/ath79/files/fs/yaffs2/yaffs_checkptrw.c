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
 */

#include "yaffs_checkptrw.h"
#include "yaffs_getblockinfo.h"
#include "yaffs_endian.h"

struct yaffs_checkpt_chunk_hdr {
	int version;
	int seq;
	u32 sum;
	u32 xor;
} ;


static int apply_chunk_offset(struct yaffs_dev *dev, int chunk)
{
	return chunk - dev->chunk_offset;
}

static int apply_block_offset(struct yaffs_dev *dev, int block)
{
	return block - dev->block_offset;
}


static void yaffs2_do_endian_hdr(struct yaffs_dev *dev,
				 struct yaffs_checkpt_chunk_hdr *hdr)
{
	if (!dev->swap_endian)
		return;
	hdr->version = swap_s32(hdr->version);
	hdr->seq     = swap_s32(hdr->seq);
	hdr->sum     = swap_u32(hdr->sum);
	hdr->xor     = swap_u32(hdr->xor);
}

static void yaffs2_checkpt_init_chunk_hdr(struct yaffs_dev *dev)
{
	struct yaffs_checkpt_chunk_hdr hdr;

	hdr.version = YAFFS_CHECKPOINT_VERSION;
	hdr.seq = dev->checkpt_page_seq;
	hdr.sum = dev->checkpt_sum;
	hdr.xor = dev->checkpt_xor;

	dev->checkpt_byte_offs = sizeof(hdr);

	yaffs2_do_endian_hdr(dev, &hdr);
	memcpy(dev->checkpt_buffer, &hdr, sizeof(hdr));
}

static int yaffs2_checkpt_check_chunk_hdr(struct yaffs_dev *dev)
{
	struct yaffs_checkpt_chunk_hdr hdr;

	memcpy(&hdr, dev->checkpt_buffer, sizeof(hdr));
	yaffs2_do_endian_hdr(dev, &hdr);

	dev->checkpt_byte_offs = sizeof(hdr);

	return hdr.version == YAFFS_CHECKPOINT_VERSION &&
		hdr.seq == dev->checkpt_page_seq &&
		hdr.sum == dev->checkpt_sum &&
		hdr.xor == dev->checkpt_xor;
}

static int yaffs2_checkpt_space_ok(struct yaffs_dev *dev)
{
	int blocks_avail = dev->n_erased_blocks - dev->param.n_reserved_blocks;

	yaffs_trace(YAFFS_TRACE_CHECKPOINT,
		"checkpt blocks_avail = %d", blocks_avail);

	return (blocks_avail <= 0) ? 0 : 1;
}

static int yaffs_checkpt_erase(struct yaffs_dev *dev)
{
	u32 i;

	if (!dev->drv.drv_erase_fn)
		return 0;
	yaffs_trace(YAFFS_TRACE_CHECKPOINT,
		"checking blocks %d to %d",
		dev->internal_start_block, dev->internal_end_block);

	for (i = dev->internal_start_block; i <= dev->internal_end_block; i++) {
		struct yaffs_block_info *bi = yaffs_get_block_info(dev, i);
		int offset_i = apply_block_offset(dev, i);
		int result;

		if (bi->block_state == YAFFS_BLOCK_STATE_CHECKPOINT) {
			yaffs_trace(YAFFS_TRACE_CHECKPOINT,
			"erasing checkpt block %d", i);

			dev->n_erasures++;

			result = dev->drv.drv_erase_fn(dev, offset_i);
			if(result) {
				bi->block_state = YAFFS_BLOCK_STATE_EMPTY;
				dev->n_erased_blocks++;
				dev->n_free_chunks +=
				    dev->param.chunks_per_block;
			} else {
				dev->drv.drv_mark_bad_fn(dev, offset_i);
				bi->block_state = YAFFS_BLOCK_STATE_DEAD;
			}
		}
	}

	dev->blocks_in_checkpt = 0;

	return 1;
}

static void yaffs2_checkpt_find_erased_block(struct yaffs_dev *dev)
{
	u32 i;
	int blocks_avail = dev->n_erased_blocks - dev->param.n_reserved_blocks;

	yaffs_trace(YAFFS_TRACE_CHECKPOINT,
		"allocating checkpt block: erased %d reserved %d avail %d next %d ",
		dev->n_erased_blocks, dev->param.n_reserved_blocks,
		blocks_avail, dev->checkpt_next_block);

	if (dev->checkpt_next_block >= 0 &&
	    dev->checkpt_next_block <= (int)dev->internal_end_block &&
	    blocks_avail > 0) {

		for (i = dev->checkpt_next_block; i <= dev->internal_end_block;
		     i++) {
			struct yaffs_block_info *bi;

			bi = yaffs_get_block_info(dev, i);
			if (bi->block_state == YAFFS_BLOCK_STATE_EMPTY) {
				dev->checkpt_next_block = i + 1;
				dev->checkpt_cur_block = i;
				yaffs_trace(YAFFS_TRACE_CHECKPOINT,
					"allocating checkpt block %d", i);
				return;
			}
		}
	}
	yaffs_trace(YAFFS_TRACE_CHECKPOINT, "out of checkpt blocks");

	dev->checkpt_next_block = -1;
	dev->checkpt_cur_block = -1;
}

static void yaffs2_checkpt_find_block(struct yaffs_dev *dev)
{
	u32 i;
	struct yaffs_ext_tags tags;

	yaffs_trace(YAFFS_TRACE_CHECKPOINT,
		"find next checkpt block: start:  blocks %d next %d",
		dev->blocks_in_checkpt, dev->checkpt_next_block);

	if (dev->blocks_in_checkpt < dev->checkpt_max_blocks)
		for (i = dev->checkpt_next_block; i <= dev->internal_end_block;
		     i++) {
			int chunk = i * dev->param.chunks_per_block;
			enum yaffs_block_state state;
			u32 seq;

			dev->tagger.read_chunk_tags_fn(dev,
					apply_chunk_offset(dev, chunk),
					NULL, &tags);
			yaffs_trace(YAFFS_TRACE_CHECKPOINT,
				"find next checkpt block: search: block %d state %d oid %d seq %d eccr %d",
				i, (int) state,
				tags.obj_id, tags.seq_number,
				tags.ecc_result);

			if (tags.seq_number != YAFFS_SEQUENCE_CHECKPOINT_DATA)
				continue;

			dev->tagger.query_block_fn(dev,
						apply_block_offset(dev, i),
						&state, &seq);
			if (state == YAFFS_BLOCK_STATE_DEAD)
				continue;

			/* Right kind of block */
			dev->checkpt_next_block = tags.obj_id;
			dev->checkpt_cur_block = i;
			dev->checkpt_block_list[dev->blocks_in_checkpt] = i;
			dev->blocks_in_checkpt++;
			yaffs_trace(YAFFS_TRACE_CHECKPOINT,
				"found checkpt block %d", i);
			return;
		}

	yaffs_trace(YAFFS_TRACE_CHECKPOINT, "found no more checkpt blocks");

	dev->checkpt_next_block = -1;
	dev->checkpt_cur_block = -1;
}

int yaffs2_checkpt_open(struct yaffs_dev *dev, int writing)
{
	u32 i;

	dev->checkpt_open_write = writing;

	/* Got the functions we need? */
	if (!dev->tagger.write_chunk_tags_fn ||
	    !dev->tagger.read_chunk_tags_fn ||
	    !dev->drv.drv_erase_fn ||
	    !dev->drv.drv_mark_bad_fn)
		return 0;

	if (writing && !yaffs2_checkpt_space_ok(dev))
		return 0;

	if (!dev->checkpt_buffer)
		dev->checkpt_buffer =
		    kmalloc(dev->param.total_bytes_per_chunk, GFP_NOFS);
	if (!dev->checkpt_buffer)
		return 0;

	dev->checkpt_page_seq = 0;
	dev->checkpt_byte_count = 0;
	dev->checkpt_sum = 0;
	dev->checkpt_xor = 0;
	dev->checkpt_cur_block = -1;
	dev->checkpt_cur_chunk = -1;
	dev->checkpt_next_block = dev->internal_start_block;

	if (writing) {
		memset(dev->checkpt_buffer, 0, dev->data_bytes_per_chunk);
		yaffs2_checkpt_init_chunk_hdr(dev);
		return yaffs_checkpt_erase(dev);
	}

	/* Opening for a read */
	/* Set to a value that will kick off a read */
	dev->checkpt_byte_offs = dev->data_bytes_per_chunk;
	/* A checkpoint block list of 1 checkpoint block per 16 block is
	 * (hopefully) going to be way more than we need */
	dev->blocks_in_checkpt = 0;
	dev->checkpt_max_blocks =
	    (dev->internal_end_block - dev->internal_start_block) / 16 + 2;
	if (!dev->checkpt_block_list)
		dev->checkpt_block_list =
		      kmalloc(sizeof(int) * dev->checkpt_max_blocks, GFP_NOFS);

	if (!dev->checkpt_block_list)
		return 0;

	for (i = 0; i < dev->checkpt_max_blocks; i++)
		dev->checkpt_block_list[i] = -1;

	return 1;
}

int yaffs2_get_checkpt_sum(struct yaffs_dev *dev, u32 * sum)
{
	u32 composite_sum;

	composite_sum = (dev->checkpt_sum << 8) | (dev->checkpt_xor & 0xff);
	*sum = composite_sum;
	return 1;
}

static int yaffs2_checkpt_flush_buffer(struct yaffs_dev *dev)
{
	int chunk;
	int offset_chunk;
	struct yaffs_ext_tags tags;

	if (dev->checkpt_cur_block < 0) {
		yaffs2_checkpt_find_erased_block(dev);
		dev->checkpt_cur_chunk = 0;
	}

	if (dev->checkpt_cur_block < 0)
		return 0;

	tags.is_deleted = 0;
	tags.obj_id = dev->checkpt_next_block;	/* Hint to next place to look */
	tags.chunk_id = dev->checkpt_page_seq + 1;
	tags.seq_number = YAFFS_SEQUENCE_CHECKPOINT_DATA;
	tags.n_bytes = dev->data_bytes_per_chunk;
	if (dev->checkpt_cur_chunk == 0) {
		/* First chunk we write for the block? Set block state to
		   checkpoint */
		struct yaffs_block_info *bi =
		    yaffs_get_block_info(dev, dev->checkpt_cur_block);
		bi->block_state = YAFFS_BLOCK_STATE_CHECKPOINT;
		dev->blocks_in_checkpt++;
	}

	chunk =
	    dev->checkpt_cur_block * dev->param.chunks_per_block +
	    dev->checkpt_cur_chunk;

	yaffs_trace(YAFFS_TRACE_CHECKPOINT,
		"checkpoint wite buffer nand %d(%d:%d) objid %d chId %d",
		chunk, dev->checkpt_cur_block, dev->checkpt_cur_chunk,
		tags.obj_id, tags.chunk_id);

	offset_chunk = apply_chunk_offset(dev, chunk);

	dev->n_page_writes++;

	dev->tagger.write_chunk_tags_fn(dev, offset_chunk,
				       dev->checkpt_buffer, &tags);
	dev->checkpt_page_seq++;
	dev->checkpt_cur_chunk++;
	if (dev->checkpt_cur_chunk >= (int)dev->param.chunks_per_block) {
		dev->checkpt_cur_chunk = 0;
		dev->checkpt_cur_block = -1;
	}
	memset(dev->checkpt_buffer, 0, dev->data_bytes_per_chunk);

	yaffs2_checkpt_init_chunk_hdr(dev);


	return 1;
}

int yaffs2_checkpt_wr(struct yaffs_dev *dev, const void *data, int n_bytes)
{
	int i = 0;
	int ok = 1;
	u8 *data_bytes = (u8 *) data;

	if (!dev->checkpt_buffer)
		return 0;

	if (!dev->checkpt_open_write)
		return -1;

	while (i < n_bytes && ok) {
		dev->checkpt_buffer[dev->checkpt_byte_offs] = *data_bytes;
		dev->checkpt_sum += *data_bytes;
		dev->checkpt_xor ^= *data_bytes;

		dev->checkpt_byte_offs++;
		i++;
		data_bytes++;
		dev->checkpt_byte_count++;

		if (dev->checkpt_byte_offs < 0 ||
		    dev->checkpt_byte_offs >= (int)dev->data_bytes_per_chunk)
			ok = yaffs2_checkpt_flush_buffer(dev);
	}

	return i;
}

int yaffs2_checkpt_rd(struct yaffs_dev *dev, void *data, int n_bytes)
{
	int i = 0;
	struct yaffs_ext_tags tags;
	int chunk;
	int offset_chunk;
	u8 *data_bytes = (u8 *) data;

	if (!dev->checkpt_buffer)
		return 0;

	if (dev->checkpt_open_write)
		return -1;

	while (i < n_bytes) {

		if (dev->checkpt_byte_offs < 0 ||
		    dev->checkpt_byte_offs >= (int)dev->data_bytes_per_chunk) {

			if (dev->checkpt_cur_block < 0) {
				yaffs2_checkpt_find_block(dev);
				dev->checkpt_cur_chunk = 0;
			}

			/* Bail out if we can't find a checpoint block */
			if (dev->checkpt_cur_block < 0)
				break;

			chunk = dev->checkpt_cur_block *
			    dev->param.chunks_per_block +
			    dev->checkpt_cur_chunk;

			offset_chunk = apply_chunk_offset(dev, chunk);
			dev->n_page_reads++;

			/* Read in the next chunk */
			dev->tagger.read_chunk_tags_fn(dev,
						offset_chunk,
						dev->checkpt_buffer,
						&tags);

			/* Bail out if the chunk is corrupted. */
			if (tags.chunk_id != (u32)(dev->checkpt_page_seq + 1) ||
			    tags.ecc_result > YAFFS_ECC_RESULT_FIXED ||
			    tags.seq_number != YAFFS_SEQUENCE_CHECKPOINT_DATA)
				break;

			/* Bail out if it is not a checkpoint chunk. */
			if(!yaffs2_checkpt_check_chunk_hdr(dev))
				break;

			dev->checkpt_page_seq++;
			dev->checkpt_cur_chunk++;

			if (dev->checkpt_cur_chunk >=
					(int)dev->param.chunks_per_block)
				dev->checkpt_cur_block = -1;

		}

		*data_bytes = dev->checkpt_buffer[dev->checkpt_byte_offs];
		dev->checkpt_sum += *data_bytes;
		dev->checkpt_xor ^= *data_bytes;
		dev->checkpt_byte_offs++;
		i++;
		data_bytes++;
		dev->checkpt_byte_count++;
	}

	return i; /* Number of bytes read */
}

int yaffs_checkpt_close(struct yaffs_dev *dev)
{
	u32 i;

	if (dev->checkpt_open_write) {
		if (dev->checkpt_byte_offs !=
			sizeof(struct yaffs_checkpt_chunk_hdr))
			yaffs2_checkpt_flush_buffer(dev);
	} else if (dev->checkpt_block_list) {
		for (i = 0;
		     i < dev->blocks_in_checkpt &&
		     dev->checkpt_block_list[i] >= 0; i++) {
			int blk = dev->checkpt_block_list[i];
			struct yaffs_block_info *bi = NULL;

			if ((int)dev->internal_start_block <= blk &&
			    blk <= (int)dev->internal_end_block)
				bi = yaffs_get_block_info(dev, blk);
			if (bi && bi->block_state == YAFFS_BLOCK_STATE_EMPTY)
				bi->block_state = YAFFS_BLOCK_STATE_CHECKPOINT;
		}
	}

	dev->n_free_chunks -=
		dev->blocks_in_checkpt * dev->param.chunks_per_block;
	dev->n_erased_blocks -= dev->blocks_in_checkpt;

	yaffs_trace(YAFFS_TRACE_CHECKPOINT, "checkpoint byte count %d",
		dev->checkpt_byte_count);

	if (dev->checkpt_buffer)
		return 1;
	else
		return 0;
}

int yaffs2_checkpt_invalidate_stream(struct yaffs_dev *dev)
{
	/* Erase the checkpoint data */

	yaffs_trace(YAFFS_TRACE_CHECKPOINT,
		"checkpoint invalidate of %d blocks",
		dev->blocks_in_checkpt);

	return yaffs_checkpt_erase(dev);
}
