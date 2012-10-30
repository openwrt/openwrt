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

/* mtd interface for YAFFS2 */

#include "yportenv.h"
#include "yaffs_trace.h"

#include "yaffs_mtdif2.h"

#include "linux/mtd/mtd.h"
#include "linux/types.h"
#include "linux/time.h"

#include "yaffs_packedtags2.h"

#include "yaffs_linux.h"

/* NB For use with inband tags....
 * We assume that the data buffer is of size totalBytersPerChunk so that we can also
 * use it to load the tags.
 */
int nandmtd2_WriteChunkWithTagsToNAND(yaffs_dev_t *dev, int nand_chunk,
				      const __u8 *data,
				      const yaffs_ext_tags *tags)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
#if (MTD_VERSION_CODE > MTD_VERSION(2, 6, 17))
	struct mtd_oob_ops ops;
#else
	size_t dummy;
#endif
	int retval = 0;

	loff_t addr;

	yaffs_PackedTags2 pt;

	int packed_tags_size = dev->param.no_tags_ecc ? sizeof(pt.t) : sizeof(pt);
	void * packed_tags_ptr = dev->param.no_tags_ecc ? (void *) &pt.t : (void *)&pt;

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_WriteChunkWithTagsToNAND chunk %d data %p tags %p"
	    TENDSTR), nand_chunk, data, tags));


	addr  = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;

	/* For yaffs2 writing there must be both data and tags.
	 * If we're using inband tags, then the tags are stuffed into
	 * the end of the data buffer.
	 */
	if (!data || !tags)
		BUG();
	else if (dev->param.inband_tags) {
		yaffs_PackedTags2TagsPart *pt2tp;
		pt2tp = (yaffs_PackedTags2TagsPart *)(data + dev->data_bytes_per_chunk);
		yaffs_PackTags2TagsPart(pt2tp, tags);
	} else
		yaffs_PackTags2(&pt, tags, !dev->param.no_tags_ecc);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
	ops.mode = MTD_OOB_AUTO;
	ops.ooblen = (dev->param.inband_tags) ? 0 : packed_tags_size;
	ops.len = dev->param.total_bytes_per_chunk;
	ops.ooboffs = 0;
	ops.datbuf = (__u8 *)data;
	ops.oobbuf = (dev->param.inband_tags) ? NULL : packed_tags_ptr;
	retval = mtd->write_oob(mtd, addr, &ops);

#else
	if (!dev->param.inband_tags) {
		retval =
		    mtd->write_ecc(mtd, addr, dev->data_bytes_per_chunk,
				   &dummy, data, (__u8 *) packed_tags_ptr, NULL);
	} else {
		retval =
		    mtd->write(mtd, addr, dev->param.total_bytes_per_chunk, &dummy,
			       data);
	}
#endif

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

int nandmtd2_ReadChunkWithTagsFromNAND(yaffs_dev_t *dev, int nand_chunk,
				       __u8 *data, yaffs_ext_tags *tags)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
#if (MTD_VERSION_CODE > MTD_VERSION(2, 6, 17))
	struct mtd_oob_ops ops;
#endif
	size_t dummy;
	int retval = 0;
	int localData = 0;

	loff_t addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;

	yaffs_PackedTags2 pt;

	int packed_tags_size = dev->param.no_tags_ecc ? sizeof(pt.t) : sizeof(pt);
	void * packed_tags_ptr = dev->param.no_tags_ecc ? (void *) &pt.t: (void *)&pt;

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_ReadChunkWithTagsFromNAND chunk %d data %p tags %p"
	    TENDSTR), nand_chunk, data, tags));

	if (dev->param.inband_tags) {

		if (!data) {
			localData = 1;
			data = yaffs_get_temp_buffer(dev, __LINE__);
		}


	}


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
	if (dev->param.inband_tags || (data && !tags))
		retval = mtd->read(mtd, addr, dev->param.total_bytes_per_chunk,
				&dummy, data);
	else if (tags) {
		ops.mode = MTD_OOB_AUTO;
		ops.ooblen = packed_tags_size;
		ops.len = data ? dev->data_bytes_per_chunk : packed_tags_size;
		ops.ooboffs = 0;
		ops.datbuf = data;
		ops.oobbuf = yaffs_dev_to_lc(dev)->spareBuffer;
		retval = mtd->read_oob(mtd, addr, &ops);
	}
#else
	if (!dev->param.inband_tags && data && tags) {

		retval = mtd->read_ecc(mtd, addr, dev->data_bytes_per_chunk,
					  &dummy, data, dev->spareBuffer,
					  NULL);
	} else {
		if (data)
			retval =
			    mtd->read(mtd, addr, dev->data_bytes_per_chunk, &dummy,
				      data);
		if (!dev->param.inband_tags && tags)
			retval =
			    mtd->read_oob(mtd, addr, mtd->oobsize, &dummy,
					  dev->spareBuffer);
	}
#endif


	if (dev->param.inband_tags) {
		if (tags) {
			yaffs_PackedTags2TagsPart *pt2tp;
			pt2tp = (yaffs_PackedTags2TagsPart *)&data[dev->data_bytes_per_chunk];
			yaffs_unpack_tags2tags_part(tags, pt2tp);
		}
	} else {
		if (tags) {
			memcpy(packed_tags_ptr, yaffs_dev_to_lc(dev)->spareBuffer, packed_tags_size);
			yaffs_unpack_tags2(tags, &pt, !dev->param.no_tags_ecc);
		}
	}

	if (localData)
		yaffs_release_temp_buffer(dev, data, __LINE__);

	if (tags && retval == -EBADMSG && tags->ecc_result == YAFFS_ECC_RESULT_NO_ERROR) {
		tags->ecc_result = YAFFS_ECC_RESULT_UNFIXED;
		dev->n_ecc_unfixed++;
	}
	if(tags && retval == -EUCLEAN && tags->ecc_result == YAFFS_ECC_RESULT_NO_ERROR) {
		tags->ecc_result = YAFFS_ECC_RESULT_FIXED;
		dev->n_ecc_fixed++;
	}
	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

int nandmtd2_MarkNANDBlockBad(struct yaffs_dev_s *dev, int block_no)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	int retval;
	T(YAFFS_TRACE_MTD,
	  (TSTR("nandmtd2_MarkNANDBlockBad %d" TENDSTR), block_no));

	retval =
	    mtd->block_markbad(mtd,
			       block_no * dev->param.chunks_per_block *
			       dev->param.total_bytes_per_chunk);

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;

}

int nandmtd2_QueryNANDBlock(struct yaffs_dev_s *dev, int block_no,
			    yaffs_block_state_t *state, __u32 *seq_number)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	int retval;

	T(YAFFS_TRACE_MTD,
	  (TSTR("nandmtd2_QueryNANDBlock %d" TENDSTR), block_no));
	retval =
	    mtd->block_isbad(mtd,
			     block_no * dev->param.chunks_per_block *
			     dev->param.total_bytes_per_chunk);

	if (retval) {
		T(YAFFS_TRACE_MTD, (TSTR("block is bad" TENDSTR)));

		*state = YAFFS_BLOCK_STATE_DEAD;
		*seq_number = 0;
	} else {
		yaffs_ext_tags t;
		nandmtd2_ReadChunkWithTagsFromNAND(dev,
						   block_no *
						   dev->param.chunks_per_block, NULL,
						   &t);

		if (t.chunk_used) {
			*seq_number = t.seq_number;
			*state = YAFFS_BLOCK_STATE_NEEDS_SCANNING;
		} else {
			*seq_number = 0;
			*state = YAFFS_BLOCK_STATE_EMPTY;
		}
	}
	T(YAFFS_TRACE_MTD,
	  (TSTR("block is bad seq %d state %d" TENDSTR), *seq_number,
	   *state));

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

