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

#include "yaffs_nand.h"
#include "yaffs_tagscompat.h"
#include "yaffs_tagsvalidity.h"

#include "yaffs_getblockinfo.h"

int yaffs_rd_chunk_tags_nand(yaffs_dev_t *dev, int nand_chunk,
					   __u8 *buffer,
					   yaffs_ext_tags *tags)
{
	int result;
	yaffs_ext_tags localTags;

	int realignedChunkInNAND = nand_chunk - dev->chunk_offset;

	dev->n_page_reads++;

	/* If there are no tags provided, use local tags to get prioritised gc working */
	if (!tags)
		tags = &localTags;

	if (dev->param.read_chunk_tags_fn)
		result = dev->param.read_chunk_tags_fn(dev, realignedChunkInNAND, buffer,
						      tags);
	else
		result = yaffs_tags_compat_rd(dev,
									realignedChunkInNAND,
									buffer,
									tags);
	if (tags &&
	   tags->ecc_result > YAFFS_ECC_RESULT_NO_ERROR) {

		yaffs_block_info_t *bi;
		bi = yaffs_get_block_info(dev, nand_chunk/dev->param.chunks_per_block);
		yaffs_handle_chunk_error(dev, bi);
	}

	return result;
}

int yaffs_wr_chunk_tags_nand(yaffs_dev_t *dev,
						   int nand_chunk,
						   const __u8 *buffer,
						   yaffs_ext_tags *tags)
{

	dev->n_page_writes++;

	nand_chunk -= dev->chunk_offset;


	if (tags) {
		tags->seq_number = dev->seq_number;
		tags->chunk_used = 1;
		if (!yaffs_validate_tags(tags)) {
			T(YAFFS_TRACE_ERROR,
			  (TSTR("Writing uninitialised tags" TENDSTR)));
			YBUG();
		}
		T(YAFFS_TRACE_WRITE,
		  (TSTR("Writing chunk %d tags %d %d" TENDSTR), nand_chunk,
		   tags->obj_id, tags->chunk_id));
	} else {
		T(YAFFS_TRACE_ERROR, (TSTR("Writing with no tags" TENDSTR)));
		YBUG();
	}

	if (dev->param.write_chunk_tags_fn)
		return dev->param.write_chunk_tags_fn(dev, nand_chunk, buffer,
						     tags);
	else
		return yaffs_tags_compat_wr(dev,
								       nand_chunk,
								       buffer,
								       tags);
}

int yaffs_mark_bad(yaffs_dev_t *dev, int block_no)
{
	block_no -= dev->block_offset;


	if (dev->param.bad_block_fn)
		return dev->param.bad_block_fn(dev, block_no);
	else
		return yaffs_tags_compat_mark_bad(dev, block_no);
}

int yaffs_query_init_block_state(yaffs_dev_t *dev,
						 int block_no,
						 yaffs_block_state_t *state,
						 __u32 *seq_number)
{
	block_no -= dev->block_offset;

	if (dev->param.query_block_fn)
		return dev->param.query_block_fn(dev, block_no, state, seq_number);
	else
		return yaffs_tags_compat_query_block(dev, block_no,
							     state,
							     seq_number);
}


int yaffs_erase_block(struct yaffs_dev_s *dev,
				  int flash_block)
{
	int result;

	flash_block -= dev->block_offset;

	dev->n_erasures++;

	result = dev->param.erase_fn(dev, flash_block);

	return result;
}

int yaffs_init_nand(struct yaffs_dev_s *dev)
{
	if(dev->param.initialise_flash_fn)
		return dev->param.initialise_flash_fn(dev);
	return YAFFS_OK;
}



