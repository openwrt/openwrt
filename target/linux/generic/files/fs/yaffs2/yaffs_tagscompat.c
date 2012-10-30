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

#include "yaffs_guts.h"
#include "yaffs_tagscompat.h"
#include "yaffs_ecc.h"
#include "yaffs_getblockinfo.h"
#include "yaffs_trace.h"

static void yaffs_handle_rd_data_error(yaffs_dev_t *dev, int nand_chunk);
#ifdef NOTYET
static void yaffs_check_written_block(yaffs_dev_t *dev, int nand_chunk);
static void yaffs_handle_chunk_wr_ok(yaffs_dev_t *dev, int nand_chunk,
				     const __u8 *data,
				     const yaffs_spare *spare);
static void yaffs_handle_chunk_update(yaffs_dev_t *dev, int nand_chunk,
				    const yaffs_spare *spare);
static void yaffs_handle_chunk_wr_error(yaffs_dev_t *dev, int nand_chunk);
#endif

static const char yaffs_count_bits_table[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

int yaffs_count_bits(__u8 x)
{
	int retVal;
	retVal = yaffs_count_bits_table[x];
	return retVal;
}

/********** Tags ECC calculations  *********/

void yaffs_calc_ecc(const __u8 *data, yaffs_spare *spare)
{
	yaffs_ecc_cacl(data, spare->ecc1);
	yaffs_ecc_cacl(&data[256], spare->ecc2);
}

void yaffs_calc_tags_ecc(yaffs_tags_t *tags)
{
	/* Calculate an ecc */

	unsigned char *b = ((yaffs_tags_union_t *) tags)->as_bytes;
	unsigned i, j;
	unsigned ecc = 0;
	unsigned bit = 0;

	tags->ecc = 0;

	for (i = 0; i < 8; i++) {
		for (j = 1; j & 0xff; j <<= 1) {
			bit++;
			if (b[i] & j)
				ecc ^= bit;
		}
	}

	tags->ecc = ecc;

}

int yaffs_check_tags_ecc(yaffs_tags_t *tags)
{
	unsigned ecc = tags->ecc;

	yaffs_calc_tags_ecc(tags);

	ecc ^= tags->ecc;

	if (ecc && ecc <= 64) {
		/* TODO: Handle the failure better. Retire? */
		unsigned char *b = ((yaffs_tags_union_t *) tags)->as_bytes;

		ecc--;

		b[ecc / 8] ^= (1 << (ecc & 7));

		/* Now recvalc the ecc */
		yaffs_calc_tags_ecc(tags);

		return 1;	/* recovered error */
	} else if (ecc) {
		/* Wierd ecc failure value */
		/* TODO Need to do somethiong here */
		return -1;	/* unrecovered error */
	}

	return 0;
}

/********** Tags **********/

static void yaffs_load_tags_to_spare(yaffs_spare *sparePtr,
				yaffs_tags_t *tagsPtr)
{
	yaffs_tags_union_t *tu = (yaffs_tags_union_t *) tagsPtr;

	yaffs_calc_tags_ecc(tagsPtr);

	sparePtr->tb0 = tu->as_bytes[0];
	sparePtr->tb1 = tu->as_bytes[1];
	sparePtr->tb2 = tu->as_bytes[2];
	sparePtr->tb3 = tu->as_bytes[3];
	sparePtr->tb4 = tu->as_bytes[4];
	sparePtr->tb5 = tu->as_bytes[5];
	sparePtr->tb6 = tu->as_bytes[6];
	sparePtr->tb7 = tu->as_bytes[7];
}

static void yaffs_get_tags_from_spare(yaffs_dev_t *dev, yaffs_spare *sparePtr,
				yaffs_tags_t *tagsPtr)
{
	yaffs_tags_union_t *tu = (yaffs_tags_union_t *) tagsPtr;
	int result;

	tu->as_bytes[0] = sparePtr->tb0;
	tu->as_bytes[1] = sparePtr->tb1;
	tu->as_bytes[2] = sparePtr->tb2;
	tu->as_bytes[3] = sparePtr->tb3;
	tu->as_bytes[4] = sparePtr->tb4;
	tu->as_bytes[5] = sparePtr->tb5;
	tu->as_bytes[6] = sparePtr->tb6;
	tu->as_bytes[7] = sparePtr->tb7;

	result = yaffs_check_tags_ecc(tagsPtr);
	if (result > 0)
		dev->n_tags_ecc_fixed++;
	else if (result < 0)
		dev->n_tags_ecc_unfixed++;
}

static void yaffs_spare_init(yaffs_spare *spare)
{
	memset(spare, 0xFF, sizeof(yaffs_spare));
}

static int yaffs_wr_nand(struct yaffs_dev_s *dev,
				int nand_chunk, const __u8 *data,
				yaffs_spare *spare)
{
	if (nand_chunk < dev->param.start_block * dev->param.chunks_per_block) {
		T(YAFFS_TRACE_ERROR,
		  (TSTR("**>> yaffs chunk %d is not valid" TENDSTR),
		   nand_chunk));
		return YAFFS_FAIL;
	}

	return dev->param.write_chunk_fn(dev, nand_chunk, data, spare);
}

static int yaffs_rd_chunk_nand(struct yaffs_dev_s *dev,
				   int nand_chunk,
				   __u8 *data,
				   yaffs_spare *spare,
				   yaffs_ecc_result *ecc_result,
				   int doErrorCorrection)
{
	int retVal;
	yaffs_spare localSpare;

	if (!spare && data) {
		/* If we don't have a real spare, then we use a local one. */
		/* Need this for the calculation of the ecc */
		spare = &localSpare;
	}

	if (!dev->param.use_nand_ecc) {
		retVal = dev->param.read_chunk_fn(dev, nand_chunk, data, spare);
		if (data && doErrorCorrection) {
			/* Do ECC correction */
			/* Todo handle any errors */
			int ecc_result1, ecc_result2;
			__u8 calcEcc[3];

			yaffs_ecc_cacl(data, calcEcc);
			ecc_result1 =
			    yaffs_ecc_correct(data, spare->ecc1, calcEcc);
			yaffs_ecc_cacl(&data[256], calcEcc);
			ecc_result2 =
			    yaffs_ecc_correct(&data[256], spare->ecc2, calcEcc);

			if (ecc_result1 > 0) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("**>>yaffs ecc error fix performed on chunk %d:0"
				    TENDSTR), nand_chunk));
				dev->n_ecc_fixed++;
			} else if (ecc_result1 < 0) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("**>>yaffs ecc error unfixed on chunk %d:0"
				    TENDSTR), nand_chunk));
				dev->n_ecc_unfixed++;
			}

			if (ecc_result2 > 0) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("**>>yaffs ecc error fix performed on chunk %d:1"
				    TENDSTR), nand_chunk));
				dev->n_ecc_fixed++;
			} else if (ecc_result2 < 0) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("**>>yaffs ecc error unfixed on chunk %d:1"
				    TENDSTR), nand_chunk));
				dev->n_ecc_unfixed++;
			}

			if (ecc_result1 || ecc_result2) {
				/* We had a data problem on this page */
				yaffs_handle_rd_data_error(dev, nand_chunk);
			}

			if (ecc_result1 < 0 || ecc_result2 < 0)
				*ecc_result = YAFFS_ECC_RESULT_UNFIXED;
			else if (ecc_result1 > 0 || ecc_result2 > 0)
				*ecc_result = YAFFS_ECC_RESULT_FIXED;
			else
				*ecc_result = YAFFS_ECC_RESULT_NO_ERROR;
		}
	} else {
		/* Must allocate enough memory for spare+2*sizeof(int) */
		/* for ecc results from device. */
		struct yaffs_nand_spare nspare;

		memset(&nspare, 0, sizeof(nspare));

		retVal = dev->param.read_chunk_fn(dev, nand_chunk, data,
					(yaffs_spare *) &nspare);
		memcpy(spare, &nspare, sizeof(yaffs_spare));
		if (data && doErrorCorrection) {
			if (nspare.eccres1 > 0) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("**>>mtd ecc error fix performed on chunk %d:0"
				    TENDSTR), nand_chunk));
			} else if (nspare.eccres1 < 0) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("**>>mtd ecc error unfixed on chunk %d:0"
				    TENDSTR), nand_chunk));
			}

			if (nspare.eccres2 > 0) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("**>>mtd ecc error fix performed on chunk %d:1"
				    TENDSTR), nand_chunk));
			} else if (nspare.eccres2 < 0) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("**>>mtd ecc error unfixed on chunk %d:1"
				    TENDSTR), nand_chunk));
			}

			if (nspare.eccres1 || nspare.eccres2) {
				/* We had a data problem on this page */
				yaffs_handle_rd_data_error(dev, nand_chunk);
			}

			if (nspare.eccres1 < 0 || nspare.eccres2 < 0)
				*ecc_result = YAFFS_ECC_RESULT_UNFIXED;
			else if (nspare.eccres1 > 0 || nspare.eccres2 > 0)
				*ecc_result = YAFFS_ECC_RESULT_FIXED;
			else
				*ecc_result = YAFFS_ECC_RESULT_NO_ERROR;

		}
	}
	return retVal;
}

#ifdef NOTYET
static int yaffs_check_chunk_erased(struct yaffs_dev_s *dev,
				  int nand_chunk)
{
	static int init;
	static __u8 cmpbuf[YAFFS_BYTES_PER_CHUNK];
	static __u8 data[YAFFS_BYTES_PER_CHUNK];
	/* Might as well always allocate the larger size for */
	/* dev->param.use_nand_ecc == true; */
	static __u8 spare[sizeof(struct yaffs_nand_spare)];

	dev->param.read_chunk_fn(dev, nand_chunk, data, (yaffs_spare *) spare);

	if (!init) {
		memset(cmpbuf, 0xff, YAFFS_BYTES_PER_CHUNK);
		init = 1;
	}

	if (memcmp(cmpbuf, data, YAFFS_BYTES_PER_CHUNK))
		return YAFFS_FAIL;
	if (memcmp(cmpbuf, spare, 16))
		return YAFFS_FAIL;

	return YAFFS_OK;

}
#endif

/*
 * Functions for robustisizing
 */

static void yaffs_handle_rd_data_error(yaffs_dev_t *dev, int nand_chunk)
{
	int flash_block = nand_chunk / dev->param.chunks_per_block;

	/* Mark the block for retirement */
	yaffs_get_block_info(dev, flash_block + dev->block_offset)->needs_retiring = 1;
	T(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
	  (TSTR("**>>Block %d marked for retirement" TENDSTR), flash_block));

	/* TODO:
	 * Just do a garbage collection on the affected block
	 * then retire the block
	 * NB recursion
	 */
}

#ifdef NOTYET
static void yaffs_check_written_block(yaffs_dev_t *dev, int nand_chunk)
{
}

static void yaffs_handle_chunk_wr_ok(yaffs_dev_t *dev, int nand_chunk,
				     const __u8 *data,
				     const yaffs_spare *spare)
{
}

static void yaffs_handle_chunk_update(yaffs_dev_t *dev, int nand_chunk,
				    const yaffs_spare *spare)
{
}

static void yaffs_handle_chunk_wr_error(yaffs_dev_t *dev, int nand_chunk)
{
	int flash_block = nand_chunk / dev->param.chunks_per_block;

	/* Mark the block for retirement */
	yaffs_get_block_info(dev, flash_block)->needs_retiring = 1;
	/* Delete the chunk */
	yaffs_chunk_del(dev, nand_chunk, 1, __LINE__);
}

static int yaffs_verify_cmp(const __u8 *d0, const __u8 *d1,
			       const yaffs_spare *s0, const yaffs_spare *s1)
{

	if (memcmp(d0, d1, YAFFS_BYTES_PER_CHUNK) != 0 ||
	    s0->tb0 != s1->tb0 ||
	    s0->tb1 != s1->tb1 ||
	    s0->tb2 != s1->tb2 ||
	    s0->tb3 != s1->tb3 ||
	    s0->tb4 != s1->tb4 ||
	    s0->tb5 != s1->tb5 ||
	    s0->tb6 != s1->tb6 ||
	    s0->tb7 != s1->tb7 ||
	    s0->ecc1[0] != s1->ecc1[0] ||
	    s0->ecc1[1] != s1->ecc1[1] ||
	    s0->ecc1[2] != s1->ecc1[2] ||
	    s0->ecc2[0] != s1->ecc2[0] ||
	    s0->ecc2[1] != s1->ecc2[1] || s0->ecc2[2] != s1->ecc2[2]) {
		return 0;
	}

	return 1;
}
#endif				/* NOTYET */

int yaffs_tags_compat_wr(yaffs_dev_t *dev,
						int nand_chunk,
						const __u8 *data,
						const yaffs_ext_tags *eTags)
{
	yaffs_spare spare;
	yaffs_tags_t tags;

	yaffs_spare_init(&spare);

	if (eTags->is_deleted)
		spare.page_status = 0;
	else {
		tags.obj_id = eTags->obj_id;
		tags.chunk_id = eTags->chunk_id;

		tags.n_bytes_lsb = eTags->n_bytes & 0x3ff;

		if (dev->data_bytes_per_chunk >= 1024)
			tags.n_bytes_msb = (eTags->n_bytes >> 10) & 3;
		else
			tags.n_bytes_msb = 3;


		tags.serial_number = eTags->serial_number;

		if (!dev->param.use_nand_ecc && data)
			yaffs_calc_ecc(data, &spare);

		yaffs_load_tags_to_spare(&spare, &tags);

	}

	return yaffs_wr_nand(dev, nand_chunk, data, &spare);
}

int yaffs_tags_compat_rd(yaffs_dev_t *dev,
						     int nand_chunk,
						     __u8 *data,
						     yaffs_ext_tags *eTags)
{

	yaffs_spare spare;
	yaffs_tags_t tags;
	yaffs_ecc_result ecc_result = YAFFS_ECC_RESULT_UNKNOWN;

	static yaffs_spare spareFF;
	static int init;

	if (!init) {
		memset(&spareFF, 0xFF, sizeof(spareFF));
		init = 1;
	}

	if (yaffs_rd_chunk_nand
	    (dev, nand_chunk, data, &spare, &ecc_result, 1)) {
		/* eTags may be NULL */
		if (eTags) {

			int deleted =
			    (yaffs_count_bits(spare.page_status) < 7) ? 1 : 0;

			eTags->is_deleted = deleted;
			eTags->ecc_result = ecc_result;
			eTags->block_bad = 0;	/* We're reading it */
			/* therefore it is not a bad block */
			eTags->chunk_used =
			    (memcmp(&spareFF, &spare, sizeof(spareFF)) !=
			     0) ? 1 : 0;

			if (eTags->chunk_used) {
				yaffs_get_tags_from_spare(dev, &spare, &tags);

				eTags->obj_id = tags.obj_id;
				eTags->chunk_id = tags.chunk_id;
				eTags->n_bytes = tags.n_bytes_lsb;

				if (dev->data_bytes_per_chunk >= 1024)
					eTags->n_bytes |= (((unsigned) tags.n_bytes_msb) << 10);

				eTags->serial_number = tags.serial_number;
			}
		}

		return YAFFS_OK;
	} else {
		return YAFFS_FAIL;
	}
}

int yaffs_tags_compat_mark_bad(struct yaffs_dev_s *dev,
					    int flash_block)
{

	yaffs_spare spare;

	memset(&spare, 0xff, sizeof(yaffs_spare));

	spare.block_status = 'Y';

	yaffs_wr_nand(dev, flash_block * dev->param.chunks_per_block, NULL,
			       &spare);
	yaffs_wr_nand(dev, flash_block * dev->param.chunks_per_block + 1,
			       NULL, &spare);

	return YAFFS_OK;

}

int yaffs_tags_compat_query_block(struct yaffs_dev_s *dev,
					  int block_no,
					  yaffs_block_state_t *state,
					  __u32 *seq_number)
{

	yaffs_spare spare0, spare1;
	static yaffs_spare spareFF;
	static int init;
	yaffs_ecc_result dummy;

	if (!init) {
		memset(&spareFF, 0xFF, sizeof(spareFF));
		init = 1;
	}

	*seq_number = 0;

	yaffs_rd_chunk_nand(dev, block_no * dev->param.chunks_per_block, NULL,
				&spare0, &dummy, 1);
	yaffs_rd_chunk_nand(dev, block_no * dev->param.chunks_per_block + 1, NULL,
				&spare1, &dummy, 1);

	if (yaffs_count_bits(spare0.block_status & spare1.block_status) < 7)
		*state = YAFFS_BLOCK_STATE_DEAD;
	else if (memcmp(&spareFF, &spare0, sizeof(spareFF)) == 0)
		*state = YAFFS_BLOCK_STATE_EMPTY;
	else
		*state = YAFFS_BLOCK_STATE_NEEDS_SCANNING;

	return YAFFS_OK;
}
