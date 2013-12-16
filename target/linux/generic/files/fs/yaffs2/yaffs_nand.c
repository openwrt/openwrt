/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
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

#include "yaffs_getblockinfo.h"
#include "yaffs_summary.h"

static int apply_chunk_offset(struct yaffs_dev *dev, int chunk)
{
	return chunk - dev->chunk_offset;
}

int yaffs_rd_chunk_tags_nand(struct yaffs_dev *dev, int nand_chunk,
			     u8 *buffer, struct yaffs_ext_tags *tags)
{
	int result;
	struct yaffs_ext_tags local_tags;
	int flash_chunk = apply_chunk_offset(dev, nand_chunk);

	dev->n_page_reads++;

	/* If there are no tags provided use local tags. */
	if (!tags)
		tags = &local_tags;

	result = dev->tagger.read_chunk_tags_fn(dev, flash_chunk, buffer, tags);
	if (tags && tags->ecc_result > YAFFS_ECC_RESULT_NO_ERROR) {

		struct yaffs_block_info *bi;
		bi = yaffs_get_block_info(dev,
					  nand_chunk /
					  dev->param.chunks_per_block);
		yaffs_handle_chunk_error(dev, bi);
	}
	return result;
}

int yaffs_wr_chunk_tags_nand(struct yaffs_dev *dev,
				int nand_chunk,
				const u8 *buffer, struct yaffs_ext_tags *tags)
{
	int result;
	int flash_chunk = apply_chunk_offset(dev, nand_chunk);

	dev->n_page_writes++;

	if (!tags) {
		yaffs_trace(YAFFS_TRACE_ERROR, "Writing with no tags");
		BUG();
		return YAFFS_FAIL;
	}

	tags->seq_number = dev->seq_number;
	tags->chunk_used = 1;
	yaffs_trace(YAFFS_TRACE_WRITE,
		"Writing chunk %d tags %d %d",
		nand_chunk, tags->obj_id, tags->chunk_id);

	result = dev->tagger.write_chunk_tags_fn(dev, flash_chunk,
							buffer, tags);

	yaffs_summary_add(dev, tags, nand_chunk);

	return result;
}

int yaffs_mark_bad(struct yaffs_dev *dev, int block_no)
{
	block_no -= dev->block_offset;
	dev->n_bad_markings++;

	if (dev->param.disable_bad_block_marking)
		return YAFFS_OK;

	return dev->tagger.mark_bad_fn(dev, block_no);
}


int yaffs_query_init_block_state(struct yaffs_dev *dev,
				 int block_no,
				 enum yaffs_block_state *state,
				 u32 *seq_number)
{
	block_no -= dev->block_offset;
	return dev->tagger.query_block_fn(dev, block_no, state, seq_number);
}

int yaffs_erase_block(struct yaffs_dev *dev, int block_no)
{
	int result;

	block_no -= dev->block_offset;
	dev->n_erasures++;
	result = dev->drv.drv_erase_fn(dev, block_no);
	return result;
}

int yaffs_init_nand(struct yaffs_dev *dev)
{
	if (dev->drv.drv_initialise_fn)
		return dev->drv.drv_initialise_fn(dev);
	return YAFFS_OK;
}

int yaffs_deinit_nand(struct yaffs_dev *dev)
{
	if (dev->drv.drv_deinitialise_fn)
		return dev->drv.drv_deinitialise_fn(dev);
	return YAFFS_OK;
}
