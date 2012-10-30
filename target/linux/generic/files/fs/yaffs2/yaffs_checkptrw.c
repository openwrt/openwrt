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

#include "yaffs_checkptrw.h"
#include "yaffs_getblockinfo.h"

static int yaffs2_checkpt_space_ok(yaffs_dev_t *dev)
{
	int blocksAvailable = dev->n_erased_blocks - dev->param.n_reserved_blocks;

	T(YAFFS_TRACE_CHECKPOINT,
		(TSTR("checkpt blocks available = %d" TENDSTR),
		blocksAvailable));

	return (blocksAvailable <= 0) ? 0 : 1;
}


static int yaffs_checkpt_erase(yaffs_dev_t *dev)
{
	int i;

	if (!dev->param.erase_fn)
		return 0;
	T(YAFFS_TRACE_CHECKPOINT, (TSTR("checking blocks %d to %d"TENDSTR),
		dev->internal_start_block, dev->internal_end_block));

	for (i = dev->internal_start_block; i <= dev->internal_end_block; i++) {
		yaffs_block_info_t *bi = yaffs_get_block_info(dev, i);
		if (bi->block_state == YAFFS_BLOCK_STATE_CHECKPOINT) {
			T(YAFFS_TRACE_CHECKPOINT, (TSTR("erasing checkpt block %d"TENDSTR), i));

			dev->n_erasures++;

			if (dev->param.erase_fn(dev, i - dev->block_offset /* realign */)) {
				bi->block_state = YAFFS_BLOCK_STATE_EMPTY;
				dev->n_erased_blocks++;
				dev->n_free_chunks += dev->param.chunks_per_block;
			} else {
				dev->param.bad_block_fn(dev, i);
				bi->block_state = YAFFS_BLOCK_STATE_DEAD;
			}
		}
	}

	dev->blocks_in_checkpt = 0;

	return 1;
}


static void yaffs2_checkpt_find_erased_block(yaffs_dev_t *dev)
{
	int  i;
	int blocksAvailable = dev->n_erased_blocks - dev->param.n_reserved_blocks;
	T(YAFFS_TRACE_CHECKPOINT,
		(TSTR("allocating checkpt block: erased %d reserved %d avail %d next %d "TENDSTR),
		dev->n_erased_blocks, dev->param.n_reserved_blocks, blocksAvailable, dev->checkpt_next_block));

	if (dev->checkpt_next_block >= 0 &&
			dev->checkpt_next_block <= dev->internal_end_block &&
			blocksAvailable > 0) {

		for (i = dev->checkpt_next_block; i <= dev->internal_end_block; i++) {
			yaffs_block_info_t *bi = yaffs_get_block_info(dev, i);
			if (bi->block_state == YAFFS_BLOCK_STATE_EMPTY) {
				dev->checkpt_next_block = i + 1;
				dev->checkpt_cur_block = i;
				T(YAFFS_TRACE_CHECKPOINT, (TSTR("allocating checkpt block %d"TENDSTR), i));
				return;
			}
		}
	}
	T(YAFFS_TRACE_CHECKPOINT, (TSTR("out of checkpt blocks"TENDSTR)));

	dev->checkpt_next_block = -1;
	dev->checkpt_cur_block = -1;
}

static void yaffs2_checkpt_find_block(yaffs_dev_t *dev)
{
	int  i;
	yaffs_ext_tags tags;

	T(YAFFS_TRACE_CHECKPOINT, (TSTR("find next checkpt block: start:  blocks %d next %d" TENDSTR),
		dev->blocks_in_checkpt, dev->checkpt_next_block));

	if (dev->blocks_in_checkpt < dev->checkpt_max_blocks)
		for (i = dev->checkpt_next_block; i <= dev->internal_end_block; i++) {
			int chunk = i * dev->param.chunks_per_block;
			int realignedChunk = chunk - dev->chunk_offset;

			dev->param.read_chunk_tags_fn(dev, realignedChunk,
					NULL, &tags);
			T(YAFFS_TRACE_CHECKPOINT, (TSTR("find next checkpt block: search: block %d oid %d seq %d eccr %d" TENDSTR),
				i, tags.obj_id, tags.seq_number, tags.ecc_result));

			if (tags.seq_number == YAFFS_SEQUENCE_CHECKPOINT_DATA) {
				/* Right kind of block */
				dev->checkpt_next_block = tags.obj_id;
				dev->checkpt_cur_block = i;
				dev->checkpt_block_list[dev->blocks_in_checkpt] = i;
				dev->blocks_in_checkpt++;
				T(YAFFS_TRACE_CHECKPOINT, (TSTR("found checkpt block %d"TENDSTR), i));
				return;
			}
		}

	T(YAFFS_TRACE_CHECKPOINT, (TSTR("found no more checkpt blocks"TENDSTR)));

	dev->checkpt_next_block = -1;
	dev->checkpt_cur_block = -1;
}


int yaffs2_checkpt_open(yaffs_dev_t *dev, int forWriting)
{


	dev->checkpt_open_write = forWriting;

	/* Got the functions we need? */
	if (!dev->param.write_chunk_tags_fn ||
		!dev->param.read_chunk_tags_fn ||
		!dev->param.erase_fn ||
		!dev->param.bad_block_fn)
		return 0;

	if (forWriting && !yaffs2_checkpt_space_ok(dev))
		return 0;

	if (!dev->checkpt_buffer)
		dev->checkpt_buffer = YMALLOC_DMA(dev->param.total_bytes_per_chunk);
	if (!dev->checkpt_buffer)
		return 0;


	dev->checkpt_page_seq = 0;
	dev->checkpt_byte_count = 0;
	dev->checkpt_sum = 0;
	dev->checkpt_xor = 0;
	dev->checkpt_cur_block = -1;
	dev->checkpt_cur_chunk = -1;
	dev->checkpt_next_block = dev->internal_start_block;

	/* Erase all the blocks in the checkpoint area */
	if (forWriting) {
		memset(dev->checkpt_buffer, 0, dev->data_bytes_per_chunk);
		dev->checkpt_byte_offs = 0;
		return yaffs_checkpt_erase(dev);
	} else {
		int i;
		/* Set to a value that will kick off a read */
		dev->checkpt_byte_offs = dev->data_bytes_per_chunk;
		/* A checkpoint block list of 1 checkpoint block per 16 block is (hopefully)
		 * going to be way more than we need */
		dev->blocks_in_checkpt = 0;
		dev->checkpt_max_blocks = (dev->internal_end_block - dev->internal_start_block)/16 + 2;
		dev->checkpt_block_list = YMALLOC(sizeof(int) * dev->checkpt_max_blocks);
		if(!dev->checkpt_block_list)
			return 0;

		for (i = 0; i < dev->checkpt_max_blocks; i++)
			dev->checkpt_block_list[i] = -1;
	}

	return 1;
}

int yaffs2_get_checkpt_sum(yaffs_dev_t *dev, __u32 *sum)
{
	__u32 compositeSum;
	compositeSum =  (dev->checkpt_sum << 8) | (dev->checkpt_xor & 0xFF);
	*sum = compositeSum;
	return 1;
}

static int yaffs2_checkpt_flush_buffer(yaffs_dev_t *dev)
{
	int chunk;
	int realignedChunk;

	yaffs_ext_tags tags;

	if (dev->checkpt_cur_block < 0) {
		yaffs2_checkpt_find_erased_block(dev);
		dev->checkpt_cur_chunk = 0;
	}

	if (dev->checkpt_cur_block < 0)
		return 0;

	tags.is_deleted = 0;
	tags.obj_id = dev->checkpt_next_block; /* Hint to next place to look */
	tags.chunk_id = dev->checkpt_page_seq + 1;
	tags.seq_number =  YAFFS_SEQUENCE_CHECKPOINT_DATA;
	tags.n_bytes = dev->data_bytes_per_chunk;
	if (dev->checkpt_cur_chunk == 0) {
		/* First chunk we write for the block? Set block state to
		   checkpoint */
		yaffs_block_info_t *bi = yaffs_get_block_info(dev, dev->checkpt_cur_block);
		bi->block_state = YAFFS_BLOCK_STATE_CHECKPOINT;
		dev->blocks_in_checkpt++;
	}

	chunk = dev->checkpt_cur_block * dev->param.chunks_per_block + dev->checkpt_cur_chunk;


	T(YAFFS_TRACE_CHECKPOINT, (TSTR("checkpoint wite buffer nand %d(%d:%d) objid %d chId %d" TENDSTR),
		chunk, dev->checkpt_cur_block, dev->checkpt_cur_chunk, tags.obj_id, tags.chunk_id));

	realignedChunk = chunk - dev->chunk_offset;

	dev->n_page_writes++;

	dev->param.write_chunk_tags_fn(dev, realignedChunk,
			dev->checkpt_buffer, &tags);
	dev->checkpt_byte_offs = 0;
	dev->checkpt_page_seq++;
	dev->checkpt_cur_chunk++;
	if (dev->checkpt_cur_chunk >= dev->param.chunks_per_block) {
		dev->checkpt_cur_chunk = 0;
		dev->checkpt_cur_block = -1;
	}
	memset(dev->checkpt_buffer, 0, dev->data_bytes_per_chunk);

	return 1;
}


int yaffs2_checkpt_wr(yaffs_dev_t *dev, const void *data, int n_bytes)
{
	int i = 0;
	int ok = 1;


	__u8 * dataBytes = (__u8 *)data;



	if (!dev->checkpt_buffer)
		return 0;

	if (!dev->checkpt_open_write)
		return -1;

	while (i < n_bytes && ok) {
		dev->checkpt_buffer[dev->checkpt_byte_offs] = *dataBytes;
		dev->checkpt_sum += *dataBytes;
		dev->checkpt_xor ^= *dataBytes;

		dev->checkpt_byte_offs++;
		i++;
		dataBytes++;
		dev->checkpt_byte_count++;


		if (dev->checkpt_byte_offs < 0 ||
		   dev->checkpt_byte_offs >= dev->data_bytes_per_chunk)
			ok = yaffs2_checkpt_flush_buffer(dev);
	}

	return i;
}

int yaffs2_checkpt_rd(yaffs_dev_t *dev, void *data, int n_bytes)
{
	int i = 0;
	int ok = 1;
	yaffs_ext_tags tags;


	int chunk;
	int realignedChunk;

	__u8 *dataBytes = (__u8 *)data;

	if (!dev->checkpt_buffer)
		return 0;

	if (dev->checkpt_open_write)
		return -1;

	while (i < n_bytes && ok) {


		if (dev->checkpt_byte_offs < 0 ||
			dev->checkpt_byte_offs >= dev->data_bytes_per_chunk) {

			if (dev->checkpt_cur_block < 0) {
				yaffs2_checkpt_find_block(dev);
				dev->checkpt_cur_chunk = 0;
			}

			if (dev->checkpt_cur_block < 0)
				ok = 0;
			else {
				chunk = dev->checkpt_cur_block *
					dev->param.chunks_per_block +
					dev->checkpt_cur_chunk;

				realignedChunk = chunk - dev->chunk_offset;
				
				dev->n_page_reads++;

				/* read in the next chunk */
				/* printf("read checkpoint page %d\n",dev->checkpointPage); */
				dev->param.read_chunk_tags_fn(dev,
						realignedChunk,
						dev->checkpt_buffer,
						&tags);

				if (tags.chunk_id != (dev->checkpt_page_seq + 1) ||
					tags.ecc_result > YAFFS_ECC_RESULT_FIXED ||
					tags.seq_number != YAFFS_SEQUENCE_CHECKPOINT_DATA)
					ok = 0;

				dev->checkpt_byte_offs = 0;
				dev->checkpt_page_seq++;
				dev->checkpt_cur_chunk++;

				if (dev->checkpt_cur_chunk >= dev->param.chunks_per_block)
					dev->checkpt_cur_block = -1;
			}
		}

		if (ok) {
			*dataBytes = dev->checkpt_buffer[dev->checkpt_byte_offs];
			dev->checkpt_sum += *dataBytes;
			dev->checkpt_xor ^= *dataBytes;
			dev->checkpt_byte_offs++;
			i++;
			dataBytes++;
			dev->checkpt_byte_count++;
		}
	}

	return 	i;
}

int yaffs_checkpt_close(yaffs_dev_t *dev)
{

	if (dev->checkpt_open_write) {
		if (dev->checkpt_byte_offs != 0)
			yaffs2_checkpt_flush_buffer(dev);
	} else if(dev->checkpt_block_list){
		int i;
		for (i = 0; i < dev->blocks_in_checkpt && dev->checkpt_block_list[i] >= 0; i++) {
			int blk = dev->checkpt_block_list[i];
			yaffs_block_info_t *bi = NULL;
			if( dev->internal_start_block <= blk && blk <= dev->internal_end_block)
				bi = yaffs_get_block_info(dev, blk);
			if (bi && bi->block_state == YAFFS_BLOCK_STATE_EMPTY)
				bi->block_state = YAFFS_BLOCK_STATE_CHECKPOINT;
			else {
				/* Todo this looks odd... */
			}
		}
		YFREE(dev->checkpt_block_list);
		dev->checkpt_block_list = NULL;
	}

	dev->n_free_chunks -= dev->blocks_in_checkpt * dev->param.chunks_per_block;
	dev->n_erased_blocks -= dev->blocks_in_checkpt;


	T(YAFFS_TRACE_CHECKPOINT, (TSTR("checkpoint byte count %d" TENDSTR),
			dev->checkpt_byte_count));

	if (dev->checkpt_buffer) {
		/* free the buffer */
		YFREE(dev->checkpt_buffer);
		dev->checkpt_buffer = NULL;
		return 1;
	} else
		return 0;
}

int yaffs2_checkpt_invalidate_stream(yaffs_dev_t *dev)
{
	/* Erase the checkpoint data */

	T(YAFFS_TRACE_CHECKPOINT, (TSTR("checkpoint invalidate of %d blocks"TENDSTR),
		dev->blocks_in_checkpt));

	return yaffs_checkpt_erase(dev);
}



