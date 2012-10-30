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
#include "yportenv.h"
#include "yaffs_trace.h"

#include "yaffsinterface.h"
#include "yaffs_guts.h"
#include "yaffs_tagsvalidity.h"
#include "yaffs_getblockinfo.h"

#include "yaffs_tagscompat.h"

#include "yaffs_nand.h"

#include "yaffs_yaffs1.h"
#include "yaffs_yaffs2.h"
#include "yaffs_bitmap.h"
#include "yaffs_verify.h"

#include "yaffs_nand.h"
#include "yaffs_packedtags2.h"

#include "yaffs_nameval.h"
#include "yaffs_allocator.h"

/* Note YAFFS_GC_GOOD_ENOUGH must be <= YAFFS_GC_PASSIVE_THRESHOLD */
#define YAFFS_GC_GOOD_ENOUGH 2
#define YAFFS_GC_PASSIVE_THRESHOLD 4

#include "yaffs_ecc.h"



/* Robustification (if it ever comes about...) */
static void yaffs_retire_block(yaffs_dev_t *dev, int flash_block);
static void yaffs_handle_chunk_wr_error(yaffs_dev_t *dev, int nand_chunk,
		int erasedOk);
static void yaffs_handle_chunk_wr_ok(yaffs_dev_t *dev, int nand_chunk,
				const __u8 *data,
				const yaffs_ext_tags *tags);
static void yaffs_handle_chunk_update(yaffs_dev_t *dev, int nand_chunk,
				const yaffs_ext_tags *tags);

/* Other local prototypes */
static void yaffs_update_parent(yaffs_obj_t *obj);
static int yaffs_unlink_obj(yaffs_obj_t *obj);
static int yaffs_obj_cache_dirty(yaffs_obj_t *obj);

static int yaffs_write_new_chunk(yaffs_dev_t *dev,
					const __u8 *buffer,
					yaffs_ext_tags *tags,
					int useReserve);


static yaffs_obj_t *yaffs_new_obj(yaffs_dev_t *dev, int number,
					yaffs_obj_type type);


static int yaffs_apply_xattrib_mod(yaffs_obj_t *obj, char *buffer, yaffs_xattr_mod *xmod);

static void yaffs_remove_obj_from_dir(yaffs_obj_t *obj);
static int yaffs_check_structures(void);
static int yaffs_generic_obj_del(yaffs_obj_t *in);

static int yaffs_check_chunk_erased(struct yaffs_dev_s *dev,
				int nand_chunk);

static int yaffs_unlink_worker(yaffs_obj_t *obj);

static int yaffs_tags_match(const yaffs_ext_tags *tags, int obj_id,
			int chunkInObject);

static int yaffs_alloc_chunk(yaffs_dev_t *dev, int useReserve,
				yaffs_block_info_t **blockUsedPtr);

static void yaffs_check_obj_details_loaded(yaffs_obj_t *in);

static void yaffs_invalidate_whole_cache(yaffs_obj_t *in);
static void yaffs_invalidate_chunk_cache(yaffs_obj_t *object, int chunk_id);

static int yaffs_find_chunk_in_file(yaffs_obj_t *in, int inode_chunk,
				yaffs_ext_tags *tags);

static int yaffs_verify_chunk_written(yaffs_dev_t *dev,
					int nand_chunk,
					const __u8 *data,
					yaffs_ext_tags *tags);


static void yaffs_load_name_from_oh(yaffs_dev_t *dev,YCHAR *name, const YCHAR *ohName, int bufferSize);
static void yaffs_load_oh_from_name(yaffs_dev_t *dev,YCHAR *ohName, const YCHAR *name);


/* Function to calculate chunk and offset */

static void yaffs_addr_to_chunk(yaffs_dev_t *dev, loff_t addr, int *chunkOut,
		__u32 *offsetOut)
{
	int chunk;
	__u32 offset;

	chunk  = (__u32)(addr >> dev->chunk_shift);

	if (dev->chunk_div == 1) {
		/* easy power of 2 case */
		offset = (__u32)(addr & dev->chunk_mask);
	} else {
		/* Non power-of-2 case */

		loff_t chunkBase;

		chunk /= dev->chunk_div;

		chunkBase = ((loff_t)chunk) * dev->data_bytes_per_chunk;
		offset = (__u32)(addr - chunkBase);
	}

	*chunkOut = chunk;
	*offsetOut = offset;
}

/* Function to return the number of shifts for a power of 2 greater than or
 * equal to the given number
 * Note we don't try to cater for all possible numbers and this does not have to
 * be hellishly efficient.
 */

static __u32 ShiftsGE(__u32 x)
{
	int extraBits;
	int nShifts;

	nShifts = extraBits = 0;

	while (x > 1) {
		if (x & 1)
			extraBits++;
		x >>= 1;
		nShifts++;
	}

	if (extraBits)
		nShifts++;

	return nShifts;
}

/* Function to return the number of shifts to get a 1 in bit 0
 */

static __u32 Shifts(__u32 x)
{
	__u32 nShifts;

	nShifts =  0;

	if (!x)
		return 0;

	while (!(x&1)) {
		x >>= 1;
		nShifts++;
	}

	return nShifts;
}



/*
 * Temporary buffer manipulations.
 */

static int yaffs_init_tmp_buffers(yaffs_dev_t *dev)
{
	int i;
	__u8 *buf = (__u8 *)1;

	memset(dev->temp_buffer, 0, sizeof(dev->temp_buffer));

	for (i = 0; buf && i < YAFFS_N_TEMP_BUFFERS; i++) {
		dev->temp_buffer[i].line = 0;	/* not in use */
		dev->temp_buffer[i].buffer = buf =
		    YMALLOC_DMA(dev->param.total_bytes_per_chunk);
	}

	return buf ? YAFFS_OK : YAFFS_FAIL;
}

__u8 *yaffs_get_temp_buffer(yaffs_dev_t *dev, int line_no)
{
	int i, j;

	dev->temp_in_use++;
	if (dev->temp_in_use > dev->max_temp)
		dev->max_temp = dev->temp_in_use;

	for (i = 0; i < YAFFS_N_TEMP_BUFFERS; i++) {
		if (dev->temp_buffer[i].line == 0) {
			dev->temp_buffer[i].line = line_no;
			if ((i + 1) > dev->max_temp) {
				dev->max_temp = i + 1;
				for (j = 0; j <= i; j++)
					dev->temp_buffer[j].max_line =
					    dev->temp_buffer[j].line;
			}

			return dev->temp_buffer[i].buffer;
		}
	}

	T(YAFFS_TRACE_BUFFERS,
	  (TSTR("Out of temp buffers at line %d, other held by lines:"),
	   line_no));
	for (i = 0; i < YAFFS_N_TEMP_BUFFERS; i++)
		T(YAFFS_TRACE_BUFFERS, (TSTR(" %d "), dev->temp_buffer[i].line));

	T(YAFFS_TRACE_BUFFERS, (TSTR(" " TENDSTR)));

	/*
	 * If we got here then we have to allocate an unmanaged one
	 * This is not good.
	 */

	dev->unmanaged_buffer_allocs++;
	return YMALLOC(dev->data_bytes_per_chunk);

}

void yaffs_release_temp_buffer(yaffs_dev_t *dev, __u8 *buffer,
				    int line_no)
{
	int i;

	dev->temp_in_use--;

	for (i = 0; i < YAFFS_N_TEMP_BUFFERS; i++) {
		if (dev->temp_buffer[i].buffer == buffer) {
			dev->temp_buffer[i].line = 0;
			return;
		}
	}

	if (buffer) {
		/* assume it is an unmanaged one. */
		T(YAFFS_TRACE_BUFFERS,
		  (TSTR("Releasing unmanaged temp buffer in line %d" TENDSTR),
		   line_no));
		YFREE(buffer);
		dev->unmanaged_buffer_deallocs++;
	}

}

/*
 * Determine if we have a managed buffer.
 */
int yaffs_is_managed_tmp_buffer(yaffs_dev_t *dev, const __u8 *buffer)
{
	int i;

	for (i = 0; i < YAFFS_N_TEMP_BUFFERS; i++) {
		if (dev->temp_buffer[i].buffer == buffer)
			return 1;
	}

	for (i = 0; i < dev->param.n_caches; i++) {
		if (dev->cache[i].data == buffer)
			return 1;
	}

	if (buffer == dev->checkpt_buffer)
		return 1;

	T(YAFFS_TRACE_ALWAYS,
		(TSTR("yaffs: unmaged buffer detected.\n" TENDSTR)));
	return 0;
}

/*
 * Verification code
 */




/*
 *  Simple hash function. Needs to have a reasonable spread
 */

static Y_INLINE int yaffs_hash_fn(int n)
{
	n = abs(n);
	return n % YAFFS_NOBJECT_BUCKETS;
}

/*
 * Access functions to useful fake objects.
 * Note that root might have a presence in NAND if permissions are set.
 */

yaffs_obj_t *yaffs_root(yaffs_dev_t *dev)
{
	return dev->root_dir;
}

yaffs_obj_t *yaffs_lost_n_found(yaffs_dev_t *dev)
{
	return dev->lost_n_found;
}


/*
 *  Erased NAND checking functions
 */

int yaffs_check_ff(__u8 *buffer, int n_bytes)
{
	/* Horrible, slow implementation */
	while (n_bytes--) {
		if (*buffer != 0xFF)
			return 0;
		buffer++;
	}
	return 1;
}

static int yaffs_check_chunk_erased(struct yaffs_dev_s *dev,
				int nand_chunk)
{
	int retval = YAFFS_OK;
	__u8 *data = yaffs_get_temp_buffer(dev, __LINE__);
	yaffs_ext_tags tags;
	int result;

	result = yaffs_rd_chunk_tags_nand(dev, nand_chunk, data, &tags);

	if (tags.ecc_result > YAFFS_ECC_RESULT_NO_ERROR)
		retval = YAFFS_FAIL;

	if (!yaffs_check_ff(data, dev->data_bytes_per_chunk) || tags.chunk_used) {
		T(YAFFS_TRACE_NANDACCESS,
		  (TSTR("Chunk %d not erased" TENDSTR), nand_chunk));
		retval = YAFFS_FAIL;
	}

	yaffs_release_temp_buffer(dev, data, __LINE__);

	return retval;

}


static int yaffs_verify_chunk_written(yaffs_dev_t *dev,
					int nand_chunk,
					const __u8 *data,
					yaffs_ext_tags *tags)
{
	int retval = YAFFS_OK;
	yaffs_ext_tags tempTags;
	__u8 *buffer = yaffs_get_temp_buffer(dev,__LINE__);
	int result;
	
	result = yaffs_rd_chunk_tags_nand(dev,nand_chunk,buffer,&tempTags);
	if(memcmp(buffer,data,dev->data_bytes_per_chunk) ||
		tempTags.obj_id != tags->obj_id ||
		tempTags.chunk_id  != tags->chunk_id ||
		tempTags.n_bytes != tags->n_bytes)
		retval = YAFFS_FAIL;

	yaffs_release_temp_buffer(dev, buffer, __LINE__);

	return retval;
}

static int yaffs_write_new_chunk(struct yaffs_dev_s *dev,
					const __u8 *data,
					yaffs_ext_tags *tags,
					int useReserve)
{
	int attempts = 0;
	int writeOk = 0;
	int chunk;

	yaffs2_checkpt_invalidate(dev);

	do {
		yaffs_block_info_t *bi = 0;
		int erasedOk = 0;

		chunk = yaffs_alloc_chunk(dev, useReserve, &bi);
		if (chunk < 0) {
			/* no space */
			break;
		}

		/* First check this chunk is erased, if it needs
		 * checking.  The checking policy (unless forced
		 * always on) is as follows:
		 *
		 * Check the first page we try to write in a block.
		 * If the check passes then we don't need to check any
		 * more.	If the check fails, we check again...
		 * If the block has been erased, we don't need to check.
		 *
		 * However, if the block has been prioritised for gc,
		 * then we think there might be something odd about
		 * this block and stop using it.
		 *
		 * Rationale: We should only ever see chunks that have
		 * not been erased if there was a partially written
		 * chunk due to power loss.  This checking policy should
		 * catch that case with very few checks and thus save a
		 * lot of checks that are most likely not needed.
		 *
		 * Mods to the above
		 * If an erase check fails or the write fails we skip the 
		 * rest of the block.
		 */

		/* let's give it a try */
		attempts++;

		if(dev->param.always_check_erased)
			bi->skip_erased_check = 0;

		if (!bi->skip_erased_check) {
			erasedOk = yaffs_check_chunk_erased(dev, chunk);
			if (erasedOk != YAFFS_OK) {
				T(YAFFS_TRACE_ERROR,
				(TSTR("**>> yaffs chunk %d was not erased"
				TENDSTR), chunk));

				/* If not erased, delete this one,
				 * skip rest of block and
				 * try another chunk */
				 yaffs_chunk_del(dev,chunk,1,__LINE__);
				 yaffs_skip_rest_of_block(dev);
				continue;
			}
		}

		writeOk = yaffs_wr_chunk_tags_nand(dev, chunk,
				data, tags);

		if(!bi->skip_erased_check)
			writeOk = yaffs_verify_chunk_written(dev, chunk, data, tags);

		if (writeOk != YAFFS_OK) {
			/* Clean up aborted write, skip to next block and
			 * try another chunk */
			yaffs_handle_chunk_wr_error(dev, chunk, erasedOk);
			continue;
		}

		bi->skip_erased_check = 1;

		/* Copy the data into the robustification buffer */
		yaffs_handle_chunk_wr_ok(dev, chunk, data, tags);

	} while (writeOk != YAFFS_OK &&
		(yaffs_wr_attempts <= 0 || attempts <= yaffs_wr_attempts));

	if (!writeOk)
		chunk = -1;

	if (attempts > 1) {
		T(YAFFS_TRACE_ERROR,
			(TSTR("**>> yaffs write required %d attempts" TENDSTR),
			attempts));

		dev->n_retired_writes += (attempts - 1);
	}

	return chunk;
}


 
/*
 * Block retiring for handling a broken block.
 */

static void yaffs_retire_block(yaffs_dev_t *dev, int flash_block)
{
	yaffs_block_info_t *bi = yaffs_get_block_info(dev, flash_block);

	yaffs2_checkpt_invalidate(dev);
	
	yaffs2_clear_oldest_dirty_seq(dev,bi);

	if (yaffs_mark_bad(dev, flash_block) != YAFFS_OK) {
		if (yaffs_erase_block(dev, flash_block) != YAFFS_OK) {
			T(YAFFS_TRACE_ALWAYS, (TSTR(
				"yaffs: Failed to mark bad and erase block %d"
				TENDSTR), flash_block));
		} else {
			yaffs_ext_tags tags;
			int chunk_id = flash_block * dev->param.chunks_per_block;

			__u8 *buffer = yaffs_get_temp_buffer(dev, __LINE__);

			memset(buffer, 0xff, dev->data_bytes_per_chunk);
			yaffs_init_tags(&tags);
			tags.seq_number = YAFFS_SEQUENCE_BAD_BLOCK;
			if (dev->param.write_chunk_tags_fn(dev, chunk_id -
				dev->chunk_offset, buffer, &tags) != YAFFS_OK)
				T(YAFFS_TRACE_ALWAYS, (TSTR("yaffs: Failed to "
					TCONT("write bad block marker to block %d")
					TENDSTR), flash_block));

			yaffs_release_temp_buffer(dev, buffer, __LINE__);
		}
	}

	bi->block_state = YAFFS_BLOCK_STATE_DEAD;
	bi->gc_prioritise = 0;
	bi->needs_retiring = 0;

	dev->n_retired_blocks++;
}

/*
 * Functions for robustisizing TODO
 *
 */

static void yaffs_handle_chunk_wr_ok(yaffs_dev_t *dev, int nand_chunk,
				const __u8 *data,
				const yaffs_ext_tags *tags)
{
	dev=dev;
	nand_chunk=nand_chunk;
	data=data;
	tags=tags;
}

static void yaffs_handle_chunk_update(yaffs_dev_t *dev, int nand_chunk,
				const yaffs_ext_tags *tags)
{
	dev=dev;
	nand_chunk=nand_chunk;
	tags=tags;
}

void yaffs_handle_chunk_error(yaffs_dev_t *dev, yaffs_block_info_t *bi)
{
	if (!bi->gc_prioritise) {
		bi->gc_prioritise = 1;
		dev->has_pending_prioritised_gc = 1;
		bi->chunk_error_strikes++;

		if (bi->chunk_error_strikes > 3) {
			bi->needs_retiring = 1; /* Too many stikes, so retire this */
			T(YAFFS_TRACE_ALWAYS, (TSTR("yaffs: Block struck out" TENDSTR)));

		}
	}
}

static void yaffs_handle_chunk_wr_error(yaffs_dev_t *dev, int nand_chunk,
		int erasedOk)
{
	int flash_block = nand_chunk / dev->param.chunks_per_block;
	yaffs_block_info_t *bi = yaffs_get_block_info(dev, flash_block);

	yaffs_handle_chunk_error(dev, bi);

	if (erasedOk) {
		/* Was an actual write failure, so mark the block for retirement  */
		bi->needs_retiring = 1;
		T(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
		  (TSTR("**>> Block %d needs retiring" TENDSTR), flash_block));
	}

	/* Delete the chunk */
	yaffs_chunk_del(dev, nand_chunk, 1, __LINE__);
	yaffs_skip_rest_of_block(dev);
}


/*---------------- Name handling functions ------------*/

static __u16 yaffs_calc_name_sum(const YCHAR *name)
{
	__u16 sum = 0;
	__u16 i = 1;

	const YUCHAR *bname = (const YUCHAR *) name;
	if (bname) {
		while ((*bname) && (i < (YAFFS_MAX_NAME_LENGTH/2))) {

#ifdef CONFIG_YAFFS_CASE_INSENSITIVE
			sum += yaffs_toupper(*bname) * i;
#else
			sum += (*bname) * i;
#endif
			i++;
			bname++;
		}
	}
	return sum;
}

void yaffs_set_obj_name(yaffs_obj_t *obj, const YCHAR *name)
{
#ifdef CONFIG_YAFFS_SHORT_NAMES_IN_RAM
	memset(obj->short_name, 0, sizeof(YCHAR) * (YAFFS_SHORT_NAME_LENGTH+1));
	if (name && yaffs_strnlen(name,YAFFS_SHORT_NAME_LENGTH+1) <= YAFFS_SHORT_NAME_LENGTH)
		yaffs_strcpy(obj->short_name, name);
	else
		obj->short_name[0] = _Y('\0');
#endif
	obj->sum = yaffs_calc_name_sum(name);
}

void yaffs_set_obj_name_from_oh(yaffs_obj_t *obj, const yaffs_obj_header *oh)
{
#ifdef CONFIG_YAFFS_AUTO_UNICODE
	YCHAR tmpName[YAFFS_MAX_NAME_LENGTH+1];
	memset(tmpName,0,sizeof(tmpName));
	yaffs_load_name_from_oh(obj->my_dev,tmpName,oh->name,YAFFS_MAX_NAME_LENGTH+1);
	yaffs_set_obj_name(obj,tmpName);
#else
	yaffs_set_obj_name(obj,oh->name);
#endif
}

/*-------------------- TNODES -------------------

 * List of spare tnodes
 * The list is hooked together using the first pointer
 * in the tnode.
 */


yaffs_tnode_t *yaffs_get_tnode(yaffs_dev_t *dev)
{
	yaffs_tnode_t *tn = yaffs_alloc_raw_tnode(dev);
	if (tn){
		memset(tn, 0, dev->tnode_size);
		dev->n_tnodes++;
	}

	dev->checkpoint_blocks_required = 0; /* force recalculation*/

	return tn;
}

/* FreeTnode frees up a tnode and puts it back on the free list */
static void yaffs_free_tnode(yaffs_dev_t *dev, yaffs_tnode_t *tn)
{
	yaffs_free_raw_tnode(dev,tn);
	dev->n_tnodes--;
	dev->checkpoint_blocks_required = 0; /* force recalculation*/
}

static void yaffs_deinit_tnodes_and_objs(yaffs_dev_t *dev)
{
	yaffs_deinit_raw_tnodes_and_objs(dev);
	dev->n_obj = 0;
	dev->n_tnodes = 0;
}


void yaffs_load_tnode_0(yaffs_dev_t *dev, yaffs_tnode_t *tn, unsigned pos,
		unsigned val)
{
	__u32 *map = (__u32 *)tn;
	__u32 bitInMap;
	__u32 bitInWord;
	__u32 wordInMap;
	__u32 mask;

	pos &= YAFFS_TNODES_LEVEL0_MASK;
	val >>= dev->chunk_grp_bits;

	bitInMap = pos * dev->tnode_width;
	wordInMap = bitInMap / 32;
	bitInWord = bitInMap & (32 - 1);

	mask = dev->tnode_mask << bitInWord;

	map[wordInMap] &= ~mask;
	map[wordInMap] |= (mask & (val << bitInWord));

	if (dev->tnode_width > (32 - bitInWord)) {
		bitInWord = (32 - bitInWord);
		wordInMap++;;
		mask = dev->tnode_mask >> (/*dev->tnode_width -*/ bitInWord);
		map[wordInMap] &= ~mask;
		map[wordInMap] |= (mask & (val >> bitInWord));
	}
}

__u32 yaffs_get_group_base(yaffs_dev_t *dev, yaffs_tnode_t *tn,
		unsigned pos)
{
	__u32 *map = (__u32 *)tn;
	__u32 bitInMap;
	__u32 bitInWord;
	__u32 wordInMap;
	__u32 val;

	pos &= YAFFS_TNODES_LEVEL0_MASK;

	bitInMap = pos * dev->tnode_width;
	wordInMap = bitInMap / 32;
	bitInWord = bitInMap & (32 - 1);

	val = map[wordInMap] >> bitInWord;

	if	(dev->tnode_width > (32 - bitInWord)) {
		bitInWord = (32 - bitInWord);
		wordInMap++;;
		val |= (map[wordInMap] << bitInWord);
	}

	val &= dev->tnode_mask;
	val <<= dev->chunk_grp_bits;

	return val;
}

/* ------------------- End of individual tnode manipulation -----------------*/

/* ---------Functions to manipulate the look-up tree (made up of tnodes) ------
 * The look up tree is represented by the top tnode and the number of top_level
 * in the tree. 0 means only the level 0 tnode is in the tree.
 */

/* FindLevel0Tnode finds the level 0 tnode, if one exists. */
yaffs_tnode_t *yaffs_find_tnode_0(yaffs_dev_t *dev,
					yaffs_file_s *file_struct,
					__u32 chunk_id)
{
	yaffs_tnode_t *tn = file_struct->top;
	__u32 i;
	int requiredTallness;
	int level = file_struct->top_level;

	dev=dev;

	/* Check sane level and chunk Id */
	if (level < 0 || level > YAFFS_TNODES_MAX_LEVEL)
		return NULL;

	if (chunk_id > YAFFS_MAX_CHUNK_ID)
		return NULL;

	/* First check we're tall enough (ie enough top_level) */

	i = chunk_id >> YAFFS_TNODES_LEVEL0_BITS;
	requiredTallness = 0;
	while (i) {
		i >>= YAFFS_TNODES_INTERNAL_BITS;
		requiredTallness++;
	}

	if (requiredTallness > file_struct->top_level)
		return NULL; /* Not tall enough, so we can't find it */

	/* Traverse down to level 0 */
	while (level > 0 && tn) {
		tn = tn->internal[(chunk_id >>
			(YAFFS_TNODES_LEVEL0_BITS +
				(level - 1) *
				YAFFS_TNODES_INTERNAL_BITS)) &
			YAFFS_TNODES_INTERNAL_MASK];
		level--;
	}

	return tn;
}

/* AddOrFindLevel0Tnode finds the level 0 tnode if it exists, otherwise first expands the tree.
 * This happens in two steps:
 *  1. If the tree isn't tall enough, then make it taller.
 *  2. Scan down the tree towards the level 0 tnode adding tnodes if required.
 *
 * Used when modifying the tree.
 *
 *  If the tn argument is NULL, then a fresh tnode will be added otherwise the specified tn will
 *  be plugged into the ttree.
 */

yaffs_tnode_t *yaffs_add_find_tnode_0(yaffs_dev_t *dev,
					yaffs_file_s *file_struct,
					__u32 chunk_id,
					yaffs_tnode_t *passed_tn)
{
	int requiredTallness;
	int i;
	int l;
	yaffs_tnode_t *tn;

	__u32 x;


	/* Check sane level and page Id */
	if (file_struct->top_level < 0 || file_struct->top_level > YAFFS_TNODES_MAX_LEVEL)
		return NULL;

	if (chunk_id > YAFFS_MAX_CHUNK_ID)
		return NULL;

	/* First check we're tall enough (ie enough top_level) */

	x = chunk_id >> YAFFS_TNODES_LEVEL0_BITS;
	requiredTallness = 0;
	while (x) {
		x >>= YAFFS_TNODES_INTERNAL_BITS;
		requiredTallness++;
	}


	if (requiredTallness > file_struct->top_level) {
		/* Not tall enough, gotta make the tree taller */
		for (i = file_struct->top_level; i < requiredTallness; i++) {

			tn = yaffs_get_tnode(dev);

			if (tn) {
				tn->internal[0] = file_struct->top;
				file_struct->top = tn;
				file_struct->top_level++;
			} else {
				T(YAFFS_TRACE_ERROR,
					(TSTR("yaffs: no more tnodes" TENDSTR)));
				return NULL;
			}
		}
	}

	/* Traverse down to level 0, adding anything we need */

	l = file_struct->top_level;
	tn = file_struct->top;

	if (l > 0) {
		while (l > 0 && tn) {
			x = (chunk_id >>
			     (YAFFS_TNODES_LEVEL0_BITS +
			      (l - 1) * YAFFS_TNODES_INTERNAL_BITS)) &
			    YAFFS_TNODES_INTERNAL_MASK;


			if ((l > 1) && !tn->internal[x]) {
				/* Add missing non-level-zero tnode */
				tn->internal[x] = yaffs_get_tnode(dev);
				if(!tn->internal[x])
					return NULL;
			} else if (l == 1) {
				/* Looking from level 1 at level 0 */
				if (passed_tn) {
					/* If we already have one, then release it.*/
					if (tn->internal[x])
						yaffs_free_tnode(dev, tn->internal[x]);
					tn->internal[x] = passed_tn;

				} else if (!tn->internal[x]) {
					/* Don't have one, none passed in */
					tn->internal[x] = yaffs_get_tnode(dev);
					if(!tn->internal[x])
						return NULL;
				}
			}

			tn = tn->internal[x];
			l--;
		}
	} else {
		/* top is level 0 */
		if (passed_tn) {
			memcpy(tn, passed_tn, (dev->tnode_width * YAFFS_NTNODES_LEVEL0)/8);
			yaffs_free_tnode(dev, passed_tn);
		}
	}

	return tn;
}

static int yaffs_find_chunk_in_group(yaffs_dev_t *dev, int theChunk,
				yaffs_ext_tags *tags, int obj_id,
				int inode_chunk)
{
	int j;

	for (j = 0; theChunk && j < dev->chunk_grp_size; j++) {
		if (yaffs_check_chunk_bit(dev, theChunk / dev->param.chunks_per_block,
				theChunk % dev->param.chunks_per_block)) {
			
			if(dev->chunk_grp_size == 1)
				return theChunk;
			else {
				yaffs_rd_chunk_tags_nand(dev, theChunk, NULL,
								tags);
				if (yaffs_tags_match(tags, obj_id, inode_chunk)) {
					/* found it; */
					return theChunk;
				}
			}
		}
		theChunk++;
	}
	return -1;
}

#if 0
/* Experimental code not being used yet. Might speed up file deletion */
/* DeleteWorker scans backwards through the tnode tree and deletes all the
 * chunks and tnodes in the file.
 * Returns 1 if the tree was deleted.
 * Returns 0 if it stopped early due to hitting the limit and the delete is incomplete.
 */

static int yaffs_del_worker(yaffs_obj_t *in, yaffs_tnode_t *tn, __u32 level,
			      int chunk_offset, int *limit)
{
	int i;
	int inode_chunk;
	int theChunk;
	yaffs_ext_tags tags;
	int foundChunk;
	yaffs_dev_t *dev = in->my_dev;

	int allDone = 1;

	if (tn) {
		if (level > 0) {
			for (i = YAFFS_NTNODES_INTERNAL - 1; allDone && i >= 0;
			     i--) {
				if (tn->internal[i]) {
					if (limit && (*limit) < 0) {
						allDone = 0;
					} else {
						allDone =
							yaffs_del_worker(in,
								tn->
								internal
								[i],
								level -
								1,
								(chunk_offset
									<<
									YAFFS_TNODES_INTERNAL_BITS)
								+ i,
								limit);
					}
					if (allDone) {
						yaffs_free_tnode(dev,
								tn->
								internal[i]);
						tn->internal[i] = NULL;
					}
				}
			}
			return (allDone) ? 1 : 0;
		} else if (level == 0) {
			int hitLimit = 0;

			for (i = YAFFS_NTNODES_LEVEL0 - 1; i >= 0 && !hitLimit;
					i--) {
				theChunk = yaffs_get_group_base(dev, tn, i);
				if (theChunk) {

					inode_chunk = (chunk_offset <<
						YAFFS_TNODES_LEVEL0_BITS) + i;

					foundChunk =
						yaffs_find_chunk_in_group(dev,
								theChunk,
								&tags,
								in->obj_id,
								inode_chunk);

					if (foundChunk > 0) {
						yaffs_chunk_del(dev,
								  foundChunk, 1,
								  __LINE__);
						in->n_data_chunks--;
						if (limit) {
							*limit = *limit - 1;
							if (*limit <= 0)
								hitLimit = 1;
						}

					}

					yaffs_load_tnode_0(dev, tn, i, 0);
				}

			}
			return (i < 0) ? 1 : 0;

		}

	}

	return 1;

}

#endif

static void yaffs_soft_del_chunk(yaffs_dev_t *dev, int chunk)
{
	yaffs_block_info_t *theBlock;
	unsigned block_no;

	T(YAFFS_TRACE_DELETION, (TSTR("soft delete chunk %d" TENDSTR), chunk));

	block_no =  chunk / dev->param.chunks_per_block;
	theBlock = yaffs_get_block_info(dev, block_no);
	if (theBlock) {
		theBlock->soft_del_pages++;
		dev->n_free_chunks++;
		yaffs2_update_oldest_dirty_seq(dev, block_no, theBlock);
	}
}

/* SoftDeleteWorker scans backwards through the tnode tree and soft deletes all the chunks in the file.
 * All soft deleting does is increment the block's softdelete count and pulls the chunk out
 * of the tnode.
 * Thus, essentially this is the same as DeleteWorker except that the chunks are soft deleted.
 */

static int yaffs_soft_del_worker(yaffs_obj_t *in, yaffs_tnode_t *tn,
				  __u32 level, int chunk_offset)
{
	int i;
	int theChunk;
	int allDone = 1;
	yaffs_dev_t *dev = in->my_dev;

	if (tn) {
		if (level > 0) {

			for (i = YAFFS_NTNODES_INTERNAL - 1; allDone && i >= 0;
			     i--) {
				if (tn->internal[i]) {
					allDone =
					    yaffs_soft_del_worker(in,
								   tn->
								   internal[i],
								   level - 1,
								   (chunk_offset
								    <<
								    YAFFS_TNODES_INTERNAL_BITS)
								   + i);
					if (allDone) {
						yaffs_free_tnode(dev,
								tn->
								internal[i]);
						tn->internal[i] = NULL;
					} else {
						/* Hoosterman... how could this happen? */
					}
				}
			}
			return (allDone) ? 1 : 0;
		} else if (level == 0) {

			for (i = YAFFS_NTNODES_LEVEL0 - 1; i >= 0; i--) {
				theChunk = yaffs_get_group_base(dev, tn, i);
				if (theChunk) {
					/* Note this does not find the real chunk, only the chunk group.
					 * We make an assumption that a chunk group is not larger than
					 * a block.
					 */
					yaffs_soft_del_chunk(dev, theChunk);
					yaffs_load_tnode_0(dev, tn, i, 0);
				}

			}
			return 1;

		}

	}

	return 1;

}

static void yaffs_soft_del_file(yaffs_obj_t *obj)
{
	if (obj->deleted &&
	    obj->variant_type == YAFFS_OBJECT_TYPE_FILE && !obj->soft_del) {
		if (obj->n_data_chunks <= 0) {
			/* Empty file with no duplicate object headers, just delete it immediately */
			yaffs_free_tnode(obj->my_dev,
					obj->variant.file_variant.top);
			obj->variant.file_variant.top = NULL;
			T(YAFFS_TRACE_TRACING,
			  (TSTR("yaffs: Deleting empty file %d" TENDSTR),
			   obj->obj_id));
			yaffs_generic_obj_del(obj);
		} else {
			yaffs_soft_del_worker(obj,
					       obj->variant.file_variant.top,
					       obj->variant.file_variant.
					       top_level, 0);
			obj->soft_del = 1;
		}
	}
}

/* Pruning removes any part of the file structure tree that is beyond the
 * bounds of the file (ie that does not point to chunks).
 *
 * A file should only get pruned when its size is reduced.
 *
 * Before pruning, the chunks must be pulled from the tree and the
 * level 0 tnode entries must be zeroed out.
 * Could also use this for file deletion, but that's probably better handled
 * by a special case.
 *
 * This function is recursive. For levels > 0 the function is called again on
 * any sub-tree. For level == 0 we just check if the sub-tree has data.
 * If there is no data in a subtree then it is pruned.
 */

static yaffs_tnode_t *yaffs_prune_worker(yaffs_dev_t *dev, yaffs_tnode_t *tn,
				__u32 level, int del0)
{
	int i;
	int hasData;

	if (tn) {
		hasData = 0;

		if(level > 0){
			for (i = 0; i < YAFFS_NTNODES_INTERNAL; i++) {
				if (tn->internal[i]) {
					tn->internal[i] =
						yaffs_prune_worker(dev, tn->internal[i],
							level - 1,
							(i == 0) ? del0 : 1);
				}

				if (tn->internal[i])
					hasData++;
			}
		} else {
			int tnode_size_u32 = dev->tnode_size/sizeof(__u32);
			__u32 *map = (__u32 *)tn;

                        for(i = 0; !hasData && i < tnode_size_u32; i++){
                                if(map[i])
                                        hasData++;
                        }
                }

		if (hasData == 0 && del0) {
			/* Free and return NULL */

			yaffs_free_tnode(dev, tn);
			tn = NULL;
		}

	}

	return tn;

}

static int yaffs_prune_tree(yaffs_dev_t *dev,
				yaffs_file_s *file_struct)
{
	int i;
	int hasData;
	int done = 0;
	yaffs_tnode_t *tn;

	if (file_struct->top_level > 0) {
		file_struct->top =
		    yaffs_prune_worker(dev, file_struct->top, file_struct->top_level, 0);

		/* Now we have a tree with all the non-zero branches NULL but the height
		 * is the same as it was.
		 * Let's see if we can trim internal tnodes to shorten the tree.
		 * We can do this if only the 0th element in the tnode is in use
		 * (ie all the non-zero are NULL)
		 */

		while (file_struct->top_level && !done) {
			tn = file_struct->top;

			hasData = 0;
			for (i = 1; i < YAFFS_NTNODES_INTERNAL; i++) {
				if (tn->internal[i])
					hasData++;
			}

			if (!hasData) {
				file_struct->top = tn->internal[0];
				file_struct->top_level--;
				yaffs_free_tnode(dev, tn);
			} else {
				done = 1;
			}
		}
	}

	return YAFFS_OK;
}

/*-------------------- End of File Structure functions.-------------------*/


/* AllocateEmptyObject gets us a clean Object. Tries to make allocate more if we run out */
static yaffs_obj_t *yaffs_alloc_empty_obj(yaffs_dev_t *dev)
{
	yaffs_obj_t *obj = yaffs_alloc_raw_obj(dev);

	if (obj) {
		dev->n_obj++;

		/* Now sweeten it up... */

		memset(obj, 0, sizeof(yaffs_obj_t));
		obj->being_created = 1;

		obj->my_dev = dev;
		obj->hdr_chunk = 0;
		obj->variant_type = YAFFS_OBJECT_TYPE_UNKNOWN;
		YINIT_LIST_HEAD(&(obj->hard_links));
		YINIT_LIST_HEAD(&(obj->hash_link));
		YINIT_LIST_HEAD(&obj->siblings);


		/* Now make the directory sane */
		if (dev->root_dir) {
			obj->parent = dev->root_dir;
			ylist_add(&(obj->siblings), &dev->root_dir->variant.dir_variant.children);
		}

		/* Add it to the lost and found directory.
		 * NB Can't put root or lostNFound in lostNFound so
		 * check if lostNFound exists first
		 */
		if (dev->lost_n_found)
			yaffs_add_obj_to_dir(dev->lost_n_found, obj);

		obj->being_created = 0;
	}

	dev->checkpoint_blocks_required = 0; /* force recalculation*/

	return obj;
}

static yaffs_obj_t *yaffs_create_fake_dir(yaffs_dev_t *dev, int number,
					       __u32 mode)
{

	yaffs_obj_t *obj =
	    yaffs_new_obj(dev, number, YAFFS_OBJECT_TYPE_DIRECTORY);
	if (obj) {
		obj->fake = 1;		/* it is fake so it might have no NAND presence... */
		obj->rename_allowed = 0;	/* ... and we're not allowed to rename it... */
		obj->unlink_allowed = 0;	/* ... or unlink it */
		obj->deleted = 0;
		obj->unlinked = 0;
		obj->yst_mode = mode;
		obj->my_dev = dev;
		obj->hdr_chunk = 0;	/* Not a valid chunk. */
	}

	return obj;

}

static void yaffs_unhash_obj(yaffs_obj_t *obj)
{
	int bucket;
	yaffs_dev_t *dev = obj->my_dev;

	/* If it is still linked into the bucket list, free from the list */
	if (!ylist_empty(&obj->hash_link)) {
		ylist_del_init(&obj->hash_link);
		bucket = yaffs_hash_fn(obj->obj_id);
		dev->obj_bucket[bucket].count--;
	}
}

/*  FreeObject frees up a Object and puts it back on the free list */
static void yaffs_free_obj(yaffs_obj_t *obj)
{
	yaffs_dev_t *dev = obj->my_dev;

	T(YAFFS_TRACE_OS, (TSTR("FreeObject %p inode %p"TENDSTR), obj, obj->my_inode));

	if (!obj)
		YBUG();
	if (obj->parent)
		YBUG();
	if (!ylist_empty(&obj->siblings))
		YBUG();


	if (obj->my_inode) {
		/* We're still hooked up to a cached inode.
		 * Don't delete now, but mark for later deletion
		 */
		obj->defered_free = 1;
		return;
	}

	yaffs_unhash_obj(obj);

	yaffs_free_raw_obj(dev,obj);
	dev->n_obj--;
	dev->checkpoint_blocks_required = 0; /* force recalculation*/
}


void yaffs_handle_defered_free(yaffs_obj_t *obj)
{
	if (obj->defered_free)
		yaffs_free_obj(obj);
}

static void yaffs_init_tnodes_and_objs(yaffs_dev_t *dev)
{
	int i;

	dev->n_obj = 0;
	dev->n_tnodes = 0;

	yaffs_init_raw_tnodes_and_objs(dev);

	for (i = 0; i < YAFFS_NOBJECT_BUCKETS; i++) {
		YINIT_LIST_HEAD(&dev->obj_bucket[i].list);
		dev->obj_bucket[i].count = 0;
	}
}

static int yaffs_find_nice_bucket(yaffs_dev_t *dev)
{
	int i;
	int l = 999;
	int lowest = 999999;


	/* Search for the shortest list or one that
	 * isn't too long.
	 */

	for (i = 0; i < 10 && lowest > 4; i++) {
		dev->bucket_finder++;
		dev->bucket_finder %= YAFFS_NOBJECT_BUCKETS;
		if (dev->obj_bucket[dev->bucket_finder].count < lowest) {
			lowest = dev->obj_bucket[dev->bucket_finder].count;
			l = dev->bucket_finder;
		}

	}

	return l;
}

static int yaffs_new_obj_id(yaffs_dev_t *dev)
{
	int bucket = yaffs_find_nice_bucket(dev);

	/* Now find an object value that has not already been taken
	 * by scanning the list.
	 */

	int found = 0;
	struct ylist_head *i;

	__u32 n = (__u32) bucket;

	/* yaffs_check_obj_hash_sane();  */

	while (!found) {
		found = 1;
		n += YAFFS_NOBJECT_BUCKETS;
		if (1 || dev->obj_bucket[bucket].count > 0) {
			ylist_for_each(i, &dev->obj_bucket[bucket].list) {
				/* If there is already one in the list */
				if (i && ylist_entry(i, yaffs_obj_t,
						hash_link)->obj_id == n) {
					found = 0;
				}
			}
		}
	}

	return n;
}

static void yaffs_hash_obj(yaffs_obj_t *in)
{
	int bucket = yaffs_hash_fn(in->obj_id);
	yaffs_dev_t *dev = in->my_dev;

	ylist_add(&in->hash_link, &dev->obj_bucket[bucket].list);
	dev->obj_bucket[bucket].count++;
}

yaffs_obj_t *yaffs_find_by_number(yaffs_dev_t *dev, __u32 number)
{
	int bucket = yaffs_hash_fn(number);
	struct ylist_head *i;
	yaffs_obj_t *in;

	ylist_for_each(i, &dev->obj_bucket[bucket].list) {
		/* Look if it is in the list */
		if (i) {
			in = ylist_entry(i, yaffs_obj_t, hash_link);
			if (in->obj_id == number) {

				/* Don't tell the VFS about this one if it is defered free */
				if (in->defered_free)
					return NULL;

				return in;
			}
		}
	}

	return NULL;
}

yaffs_obj_t *yaffs_new_obj(yaffs_dev_t *dev, int number,
				    yaffs_obj_type type)
{
	yaffs_obj_t *theObject=NULL;
	yaffs_tnode_t *tn = NULL;

	if (number < 0)
		number = yaffs_new_obj_id(dev);

	if (type == YAFFS_OBJECT_TYPE_FILE) {
		tn = yaffs_get_tnode(dev);
		if (!tn)
			return NULL;
	}

	theObject = yaffs_alloc_empty_obj(dev);
	if (!theObject){
		if(tn)
			yaffs_free_tnode(dev,tn);
		return NULL;
	}


	if (theObject) {
		theObject->fake = 0;
		theObject->rename_allowed = 1;
		theObject->unlink_allowed = 1;
		theObject->obj_id = number;
		yaffs_hash_obj(theObject);
		theObject->variant_type = type;
#ifdef CONFIG_YAFFS_WINCE
		yfsd_win_file_time_now(theObject->win_atime);
		theObject->win_ctime[0] = theObject->win_mtime[0] =
		    theObject->win_atime[0];
		theObject->win_ctime[1] = theObject->win_mtime[1] =
		    theObject->win_atime[1];

#else

		theObject->yst_atime = theObject->yst_mtime =
		    theObject->yst_ctime = Y_CURRENT_TIME;
#endif
		switch (type) {
		case YAFFS_OBJECT_TYPE_FILE:
			theObject->variant.file_variant.file_size = 0;
			theObject->variant.file_variant.scanned_size = 0;
			theObject->variant.file_variant.shrink_size = 0xFFFFFFFF;	/* max __u32 */
			theObject->variant.file_variant.top_level = 0;
			theObject->variant.file_variant.top = tn;
			break;
		case YAFFS_OBJECT_TYPE_DIRECTORY:
			YINIT_LIST_HEAD(&theObject->variant.dir_variant.
					children);
			YINIT_LIST_HEAD(&theObject->variant.dir_variant.
					dirty);
			break;
		case YAFFS_OBJECT_TYPE_SYMLINK:
		case YAFFS_OBJECT_TYPE_HARDLINK:
		case YAFFS_OBJECT_TYPE_SPECIAL:
			/* No action required */
			break;
		case YAFFS_OBJECT_TYPE_UNKNOWN:
			/* todo this should not happen */
			break;
		}
	}

	return theObject;
}

yaffs_obj_t *yaffs_find_or_create_by_number(yaffs_dev_t *dev,
						int number,
						yaffs_obj_type type)
{
	yaffs_obj_t *theObject = NULL;

	if (number > 0)
		theObject = yaffs_find_by_number(dev, number);

	if (!theObject)
		theObject = yaffs_new_obj(dev, number, type);

	return theObject;

}


YCHAR *yaffs_clone_str(const YCHAR *str)
{
	YCHAR *newStr = NULL;
	int len;

	if (!str)
		str = _Y("");

	len = yaffs_strnlen(str,YAFFS_MAX_ALIAS_LENGTH);
	newStr = YMALLOC((len + 1) * sizeof(YCHAR));
	if (newStr){
		yaffs_strncpy(newStr, str,len);
		newStr[len] = 0;
	}
	return newStr;

}

/*
 * Mknod (create) a new object.
 * equiv_obj only has meaning for a hard link;
 * aliasString only has meaning for a symlink.
 * rdev only has meaning for devices (a subset of special objects)
 */

static yaffs_obj_t *yaffs_create_obj(yaffs_obj_type type,
				       yaffs_obj_t *parent,
				       const YCHAR *name,
				       __u32 mode,
				       __u32 uid,
				       __u32 gid,
				       yaffs_obj_t *equiv_obj,
				       const YCHAR *aliasString, __u32 rdev)
{
	yaffs_obj_t *in;
	YCHAR *str = NULL;

	yaffs_dev_t *dev = parent->my_dev;

	/* Check if the entry exists. If it does then fail the call since we don't want a dup.*/
	if (yaffs_find_by_name(parent, name))
		return NULL;

	if (type == YAFFS_OBJECT_TYPE_SYMLINK) {
		str = yaffs_clone_str(aliasString);
		if (!str)
			return NULL;
	}

	in = yaffs_new_obj(dev, -1, type);

	if (!in){
		if(str)
			YFREE(str);
		return NULL;
	}





	if (in) {
		in->hdr_chunk = 0;
		in->valid = 1;
		in->variant_type = type;

		in->yst_mode = mode;

#ifdef CONFIG_YAFFS_WINCE
		yfsd_win_file_time_now(in->win_atime);
		in->win_ctime[0] = in->win_mtime[0] = in->win_atime[0];
		in->win_ctime[1] = in->win_mtime[1] = in->win_atime[1];

#else
		in->yst_atime = in->yst_mtime = in->yst_ctime = Y_CURRENT_TIME;

		in->yst_rdev = rdev;
		in->yst_uid = uid;
		in->yst_gid = gid;
#endif
		in->n_data_chunks = 0;

		yaffs_set_obj_name(in, name);
		in->dirty = 1;

		yaffs_add_obj_to_dir(parent, in);

		in->my_dev = parent->my_dev;

		switch (type) {
		case YAFFS_OBJECT_TYPE_SYMLINK:
			in->variant.symlink_variant.alias = str;
			break;
		case YAFFS_OBJECT_TYPE_HARDLINK:
			in->variant.hardlink_variant.equiv_obj =
				equiv_obj;
			in->variant.hardlink_variant.equiv_id =
				equiv_obj->obj_id;
			ylist_add(&in->hard_links, &equiv_obj->hard_links);
			break;
		case YAFFS_OBJECT_TYPE_FILE:
		case YAFFS_OBJECT_TYPE_DIRECTORY:
		case YAFFS_OBJECT_TYPE_SPECIAL:
		case YAFFS_OBJECT_TYPE_UNKNOWN:
			/* do nothing */
			break;
		}

		if (yaffs_update_oh(in, name, 0, 0, 0, NULL) < 0) {
			/* Could not create the object header, fail the creation */
			yaffs_del_obj(in);
			in = NULL;
		}

		yaffs_update_parent(parent);
	}

	return in;
}

yaffs_obj_t *yaffs_create_file(yaffs_obj_t *parent, const YCHAR *name,
			__u32 mode, __u32 uid, __u32 gid)
{
	return yaffs_create_obj(YAFFS_OBJECT_TYPE_FILE, parent, name, mode,
				uid, gid, NULL, NULL, 0);
}

yaffs_obj_t *yaffs_create_dir(yaffs_obj_t *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid)
{
	return yaffs_create_obj(YAFFS_OBJECT_TYPE_DIRECTORY, parent, name,
				 mode, uid, gid, NULL, NULL, 0);
}

yaffs_obj_t *yaffs_create_special(yaffs_obj_t *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid, __u32 rdev)
{
	return yaffs_create_obj(YAFFS_OBJECT_TYPE_SPECIAL, parent, name, mode,
				 uid, gid, NULL, NULL, rdev);
}

yaffs_obj_t *yaffs_create_symlink(yaffs_obj_t *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid,
				const YCHAR *alias)
{
	return yaffs_create_obj(YAFFS_OBJECT_TYPE_SYMLINK, parent, name, mode,
				uid, gid, NULL, alias, 0);
}

/* yaffs_link_obj returns the object id of the equivalent object.*/
yaffs_obj_t *yaffs_link_obj(yaffs_obj_t *parent, const YCHAR *name,
			yaffs_obj_t *equiv_obj)
{
	/* Get the real object in case we were fed a hard link as an equivalent object */
	equiv_obj = yaffs_get_equivalent_obj(equiv_obj);

	if (yaffs_create_obj
	    (YAFFS_OBJECT_TYPE_HARDLINK, parent, name, 0, 0, 0,
	     equiv_obj, NULL, 0)) {
		return equiv_obj;
	} else {
		return NULL;
	}

}

static int yaffs_change_obj_name(yaffs_obj_t *obj, yaffs_obj_t *new_dir,
				const YCHAR *new_name, int force, int shadows)
{
	int unlinkOp;
	int deleteOp;

	yaffs_obj_t *existingTarget;

	if (new_dir == NULL)
		new_dir = obj->parent;	/* use the old directory */

	if (new_dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("tragedy: yaffs_change_obj_name: new_dir is not a directory"
		    TENDSTR)));
		YBUG();
	}

	/* TODO: Do we need this different handling for YAFFS2 and YAFFS1?? */
	if (obj->my_dev->param.is_yaffs2)
		unlinkOp = (new_dir == obj->my_dev->unlinked_dir);
	else
		unlinkOp = (new_dir == obj->my_dev->unlinked_dir
			    && obj->variant_type == YAFFS_OBJECT_TYPE_FILE);

	deleteOp = (new_dir == obj->my_dev->del_dir);

	existingTarget = yaffs_find_by_name(new_dir, new_name);

	/* If the object is a file going into the unlinked directory,
	 *   then it is OK to just stuff it in since duplicate names are allowed.
	 *   else only proceed if the new name does not exist and if we're putting
	 *   it into a directory.
	 */
	if ((unlinkOp ||
	     deleteOp ||
	     force ||
	     (shadows > 0) ||
	     !existingTarget) &&
	    new_dir->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY) {
		yaffs_set_obj_name(obj, new_name);
		obj->dirty = 1;

		yaffs_add_obj_to_dir(new_dir, obj);

		if (unlinkOp)
			obj->unlinked = 1;

		/* If it is a deletion then we mark it as a shrink for gc purposes. */
		if (yaffs_update_oh(obj, new_name, 0, deleteOp, shadows, NULL) >= 0)
			return YAFFS_OK;
	}

	return YAFFS_FAIL;
}

int yaffs_rename_obj(yaffs_obj_t *old_dir, const YCHAR *old_name,
		yaffs_obj_t *new_dir, const YCHAR *new_name)
{
	yaffs_obj_t *obj = NULL;
	yaffs_obj_t *existingTarget = NULL;
	int force = 0;
	int result;
	yaffs_dev_t *dev;


	if (!old_dir || old_dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY)
		YBUG();
	if (!new_dir || new_dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY)
		YBUG();

	dev = old_dir->my_dev;

#ifdef CONFIG_YAFFS_CASE_INSENSITIVE
	/* Special case for case insemsitive systems (eg. WinCE).
	 * While look-up is case insensitive, the name isn't.
	 * Therefore we might want to change x.txt to X.txt
	*/
	if (old_dir == new_dir && yaffs_strcmp(old_name, new_name) == 0)
		force = 1;
#endif

	if(yaffs_strnlen(new_name,YAFFS_MAX_NAME_LENGTH+1) > YAFFS_MAX_NAME_LENGTH)
		/* ENAMETOOLONG */
		return YAFFS_FAIL;

	obj = yaffs_find_by_name(old_dir, old_name);

	if (obj && obj->rename_allowed) {

		/* Now do the handling for an existing target, if there is one */

		existingTarget = yaffs_find_by_name(new_dir, new_name);
		if (existingTarget &&
			existingTarget->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY &&
			!ylist_empty(&existingTarget->variant.dir_variant.children)) {
			/* There is a target that is a non-empty directory, so we fail */
			return YAFFS_FAIL;	/* EEXIST or ENOTEMPTY */
		} else if (existingTarget && existingTarget != obj) {
			/* Nuke the target first, using shadowing,
			 * but only if it isn't the same object.
			 *
			 * Note we must disable gc otherwise it can mess up the shadowing.
			 *
			 */
			dev->gc_disable=1;
			yaffs_change_obj_name(obj, new_dir, new_name, force,
						existingTarget->obj_id);
			existingTarget->is_shadowed = 1;
			yaffs_unlink_obj(existingTarget);
			dev->gc_disable=0;
		}

		result = yaffs_change_obj_name(obj, new_dir, new_name, 1, 0);

		yaffs_update_parent(old_dir);
		if(new_dir != old_dir)
			yaffs_update_parent(new_dir);
		
		return result;
	}
	return YAFFS_FAIL;
}

/*------------------------- Block Management and Page Allocation ----------------*/

static int yaffs_init_blocks(yaffs_dev_t *dev)
{
	int nBlocks = dev->internal_end_block - dev->internal_start_block + 1;

	dev->block_info = NULL;
	dev->chunk_bits = NULL;

	dev->alloc_block = -1;	/* force it to get a new one */

	/* If the first allocation strategy fails, thry the alternate one */
	dev->block_info = YMALLOC(nBlocks * sizeof(yaffs_block_info_t));
	if (!dev->block_info) {
		dev->block_info = YMALLOC_ALT(nBlocks * sizeof(yaffs_block_info_t));
		dev->block_info_alt = 1;
	} else
		dev->block_info_alt = 0;

	if (dev->block_info) {
		/* Set up dynamic blockinfo stuff. */
		dev->chunk_bit_stride = (dev->param.chunks_per_block + 7) / 8; /* round up bytes */
		dev->chunk_bits = YMALLOC(dev->chunk_bit_stride * nBlocks);
		if (!dev->chunk_bits) {
			dev->chunk_bits = YMALLOC_ALT(dev->chunk_bit_stride * nBlocks);
			dev->chunk_bits_alt = 1;
		} else
			dev->chunk_bits_alt = 0;
	}

	if (dev->block_info && dev->chunk_bits) {
		memset(dev->block_info, 0, nBlocks * sizeof(yaffs_block_info_t));
		memset(dev->chunk_bits, 0, dev->chunk_bit_stride * nBlocks);
		return YAFFS_OK;
	}

	return YAFFS_FAIL;
}

static void yaffs_deinit_blocks(yaffs_dev_t *dev)
{
	if (dev->block_info_alt && dev->block_info)
		YFREE_ALT(dev->block_info);
	else if (dev->block_info)
		YFREE(dev->block_info);

	dev->block_info_alt = 0;

	dev->block_info = NULL;

	if (dev->chunk_bits_alt && dev->chunk_bits)
		YFREE_ALT(dev->chunk_bits);
	else if (dev->chunk_bits)
		YFREE(dev->chunk_bits);
	dev->chunk_bits_alt = 0;
	dev->chunk_bits = NULL;
}

void yaffs_block_became_dirty(yaffs_dev_t *dev, int block_no)
{
	yaffs_block_info_t *bi = yaffs_get_block_info(dev, block_no);

	int erasedOk = 0;

	/* If the block is still healthy erase it and mark as clean.
	 * If the block has had a data failure, then retire it.
	 */

	T(YAFFS_TRACE_GC | YAFFS_TRACE_ERASE,
		(TSTR("yaffs_block_became_dirty block %d state %d %s"TENDSTR),
		block_no, bi->block_state, (bi->needs_retiring) ? "needs retiring" : ""));

	yaffs2_clear_oldest_dirty_seq(dev,bi);

	bi->block_state = YAFFS_BLOCK_STATE_DIRTY;

	/* If this is the block being garbage collected then stop gc'ing this block */
	if(block_no == dev->gc_block)
		dev->gc_block = 0;

	/* If this block is currently the best candidate for gc then drop as a candidate */
	if(block_no == dev->gc_dirtiest){
		dev->gc_dirtiest = 0;
		dev->gc_pages_in_use = 0;
	}

	if (!bi->needs_retiring) {
		yaffs2_checkpt_invalidate(dev);
		erasedOk = yaffs_erase_block(dev, block_no);
		if (!erasedOk) {
			dev->n_erase_failures++;
			T(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
			  (TSTR("**>> Erasure failed %d" TENDSTR), block_no));
		}
	}

	if (erasedOk &&
	    ((yaffs_trace_mask & YAFFS_TRACE_ERASE) || !yaffs_skip_verification(dev))) {
		int i;
		for (i = 0; i < dev->param.chunks_per_block; i++) {
			if (!yaffs_check_chunk_erased
			    (dev, block_no * dev->param.chunks_per_block + i)) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   (">>Block %d erasure supposedly OK, but chunk %d not erased"
				    TENDSTR), block_no, i));
			}
		}
	}

	if (erasedOk) {
		/* Clean it up... */
		bi->block_state = YAFFS_BLOCK_STATE_EMPTY;
		bi->seq_number = 0;
		dev->n_erased_blocks++;
		bi->pages_in_use = 0;
		bi->soft_del_pages = 0;
		bi->has_shrink_hdr = 0;
		bi->skip_erased_check = 1;  /* This is clean, so no need to check */
		bi->gc_prioritise = 0;
		yaffs_clear_chunk_bits(dev, block_no);

		T(YAFFS_TRACE_ERASE,
		  (TSTR("Erased block %d" TENDSTR), block_no));
	} else {
		dev->n_free_chunks -= dev->param.chunks_per_block;	/* We lost a block of free space */

		yaffs_retire_block(dev, block_no);
		T(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
		  (TSTR("**>> Block %d retired" TENDSTR), block_no));
	}
}

static int yaffs_find_alloc_block(yaffs_dev_t *dev)
{
	int i;

	yaffs_block_info_t *bi;

	if (dev->n_erased_blocks < 1) {
		/* Hoosterman we've got a problem.
		 * Can't get space to gc
		 */
		T(YAFFS_TRACE_ERROR,
		  (TSTR("yaffs tragedy: no more erased blocks" TENDSTR)));

		return -1;
	}

	/* Find an empty block. */

	for (i = dev->internal_start_block; i <= dev->internal_end_block; i++) {
		dev->alloc_block_finder++;
		if (dev->alloc_block_finder < dev->internal_start_block
		    || dev->alloc_block_finder > dev->internal_end_block) {
			dev->alloc_block_finder = dev->internal_start_block;
		}

		bi = yaffs_get_block_info(dev, dev->alloc_block_finder);

		if (bi->block_state == YAFFS_BLOCK_STATE_EMPTY) {
			bi->block_state = YAFFS_BLOCK_STATE_ALLOCATING;
			dev->seq_number++;
			bi->seq_number = dev->seq_number;
			dev->n_erased_blocks--;
			T(YAFFS_TRACE_ALLOCATE,
			  (TSTR("Allocated block %d, seq  %d, %d left" TENDSTR),
			   dev->alloc_block_finder, dev->seq_number,
			   dev->n_erased_blocks));
			return dev->alloc_block_finder;
		}
	}

	T(YAFFS_TRACE_ALWAYS,
	  (TSTR
	   ("yaffs tragedy: no more erased blocks, but there should have been %d"
	    TENDSTR), dev->n_erased_blocks));

	return -1;
}


/*
 * Check if there's space to allocate...
 * Thinks.... do we need top make this ths same as yaffs_get_free_chunks()?
 */
int yaffs_check_alloc_available(yaffs_dev_t *dev, int n_chunks)
{
	int reservedChunks;
	int reservedBlocks = dev->param.n_reserved_blocks;
	int checkpointBlocks;

	checkpointBlocks = yaffs_calc_checkpt_blocks_required(dev);

	reservedChunks = ((reservedBlocks + checkpointBlocks) * dev->param.chunks_per_block);

	return (dev->n_free_chunks > (reservedChunks + n_chunks));
}

static int yaffs_alloc_chunk(yaffs_dev_t *dev, int useReserve,
		yaffs_block_info_t **blockUsedPtr)
{
	int retVal;
	yaffs_block_info_t *bi;

	if (dev->alloc_block < 0) {
		/* Get next block to allocate off */
		dev->alloc_block = yaffs_find_alloc_block(dev);
		dev->alloc_page = 0;
	}

	if (!useReserve && !yaffs_check_alloc_available(dev, 1)) {
		/* Not enough space to allocate unless we're allowed to use the reserve. */
		return -1;
	}

	if (dev->n_erased_blocks < dev->param.n_reserved_blocks
			&& dev->alloc_page == 0) {
		T(YAFFS_TRACE_ALLOCATE, (TSTR("Allocating reserve" TENDSTR)));
	}

	/* Next page please.... */
	if (dev->alloc_block >= 0) {
		bi = yaffs_get_block_info(dev, dev->alloc_block);

		retVal = (dev->alloc_block * dev->param.chunks_per_block) +
			dev->alloc_page;
		bi->pages_in_use++;
		yaffs_set_chunk_bit(dev, dev->alloc_block,
				dev->alloc_page);

		dev->alloc_page++;

		dev->n_free_chunks--;

		/* If the block is full set the state to full */
		if (dev->alloc_page >= dev->param.chunks_per_block) {
			bi->block_state = YAFFS_BLOCK_STATE_FULL;
			dev->alloc_block = -1;
		}

		if (blockUsedPtr)
			*blockUsedPtr = bi;

		return retVal;
	}

	T(YAFFS_TRACE_ERROR,
			(TSTR("!!!!!!!!! Allocator out !!!!!!!!!!!!!!!!!" TENDSTR)));

	return -1;
}

static int yaffs_get_erased_chunks(yaffs_dev_t *dev)
{
	int n;

	n = dev->n_erased_blocks * dev->param.chunks_per_block;

	if (dev->alloc_block > 0)
		n += (dev->param.chunks_per_block - dev->alloc_page);

	return n;

}

/*
 * yaffs_skip_rest_of_block() skips over the rest of the allocation block
 * if we don't want to write to it.
 */
void yaffs_skip_rest_of_block(yaffs_dev_t *dev)
{
	if(dev->alloc_block > 0){
		yaffs_block_info_t *bi = yaffs_get_block_info(dev, dev->alloc_block);
		if(bi->block_state == YAFFS_BLOCK_STATE_ALLOCATING){
			bi->block_state = YAFFS_BLOCK_STATE_FULL;
			dev->alloc_block = -1;
		}
	}
}


static int yaffs_gc_block(yaffs_dev_t *dev, int block,
		int wholeBlock)
{
	int oldChunk;
	int newChunk;
	int mark_flash;
	int retVal = YAFFS_OK;
	int i;
	int isCheckpointBlock;
	int matchingChunk;
	int maxCopies;

	int chunksBefore = yaffs_get_erased_chunks(dev);
	int chunksAfter;

	yaffs_ext_tags tags;

	yaffs_block_info_t *bi = yaffs_get_block_info(dev, block);

	yaffs_obj_t *object;

	isCheckpointBlock = (bi->block_state == YAFFS_BLOCK_STATE_CHECKPOINT);


	T(YAFFS_TRACE_TRACING,
			(TSTR("Collecting block %d, in use %d, shrink %d, wholeBlock %d" TENDSTR),
			 block,
			 bi->pages_in_use,
			 bi->has_shrink_hdr,
			 wholeBlock));

	/*yaffs_verify_free_chunks(dev); */

	if(bi->block_state == YAFFS_BLOCK_STATE_FULL)
		bi->block_state = YAFFS_BLOCK_STATE_COLLECTING;
	
	bi->has_shrink_hdr = 0;	/* clear the flag so that the block can erase */

	dev->gc_disable = 1;

	if (isCheckpointBlock ||
			!yaffs_still_some_chunks(dev, block)) {
		T(YAFFS_TRACE_TRACING,
				(TSTR
				 ("Collecting block %d that has no chunks in use" TENDSTR),
				 block));
		yaffs_block_became_dirty(dev, block);
	} else {

		__u8 *buffer = yaffs_get_temp_buffer(dev, __LINE__);

		yaffs_verify_blk(dev, bi, block);

		maxCopies = (wholeBlock) ? dev->param.chunks_per_block : 5;
		oldChunk = block * dev->param.chunks_per_block + dev->gc_chunk;

		for (/* init already done */;
		     retVal == YAFFS_OK &&
		     dev->gc_chunk < dev->param.chunks_per_block &&
		     (bi->block_state == YAFFS_BLOCK_STATE_COLLECTING) &&
		     maxCopies > 0;
		     dev->gc_chunk++, oldChunk++) {
			if (yaffs_check_chunk_bit(dev, block, dev->gc_chunk)) {

				/* This page is in use and might need to be copied off */

				maxCopies--;

				mark_flash = 1;

				yaffs_init_tags(&tags);

				yaffs_rd_chunk_tags_nand(dev, oldChunk,
								buffer, &tags);

				object =
				    yaffs_find_by_number(dev,
							     tags.obj_id);

				T(YAFFS_TRACE_GC_DETAIL,
				  (TSTR
				   ("Collecting chunk in block %d, %d %d %d " TENDSTR),
				   dev->gc_chunk, tags.obj_id, tags.chunk_id,
				   tags.n_bytes));

				if (object && !yaffs_skip_verification(dev)) {
					if (tags.chunk_id == 0)
						matchingChunk = object->hdr_chunk;
					else if (object->soft_del)
						matchingChunk = oldChunk; /* Defeat the test */
					else
						matchingChunk = yaffs_find_chunk_in_file(object, tags.chunk_id, NULL);

					if (oldChunk != matchingChunk)
						T(YAFFS_TRACE_ERROR,
						  (TSTR("gc: page in gc mismatch: %d %d %d %d"TENDSTR),
						  oldChunk, matchingChunk, tags.obj_id, tags.chunk_id));

				}

				if (!object) {
					T(YAFFS_TRACE_ERROR,
					  (TSTR
					   ("page %d in gc has no object: %d %d %d "
					    TENDSTR), oldChunk,
					    tags.obj_id, tags.chunk_id, tags.n_bytes));
				}

				if (object &&
				    object->deleted &&
				    object->soft_del &&
				    tags.chunk_id != 0) {
					/* Data chunk in a soft deleted file, throw it away
					 * It's a soft deleted data chunk,
					 * No need to copy this, just forget about it and
					 * fix up the object.
					 */
					 
					/* Free chunks already includes softdeleted chunks.
					 * How ever this chunk is going to soon be really deleted
					 * which will increment free chunks.
					 * We have to decrement free chunks so this works out properly.
					 */
					dev->n_free_chunks--;
					bi->soft_del_pages--;

					object->n_data_chunks--;

					if (object->n_data_chunks <= 0) {
						/* remeber to clean up the object */
						dev->gc_cleanup_list[dev->n_clean_ups] =
						    tags.obj_id;
						dev->n_clean_ups++;
					}
					mark_flash = 0;
				} else if (0) {
					/* Todo object && object->deleted && object->n_data_chunks == 0 */
					/* Deleted object header with no data chunks.
					 * Can be discarded and the file deleted.
					 */
					object->hdr_chunk = 0;
					yaffs_free_tnode(object->my_dev,
							object->variant.
							file_variant.top);
					object->variant.file_variant.top = NULL;
					yaffs_generic_obj_del(object);

				} else if (object) {
					/* It's either a data chunk in a live file or
					 * an ObjectHeader, so we're interested in it.
					 * NB Need to keep the ObjectHeaders of deleted files
					 * until the whole file has been deleted off
					 */
					tags.serial_number++;

					dev->n_gc_copies++;

					if (tags.chunk_id == 0) {
						/* It is an object Id,
						 * We need to nuke the shrinkheader flags first
						 * Also need to clean up shadowing.
						 * We no longer want the shrinkHeader flag since its work is done
						 * and if it is left in place it will mess up scanning.
						 */

						yaffs_obj_header *oh;
						oh = (yaffs_obj_header *)buffer;

						oh->is_shrink = 0;
						tags.extra_is_shrink = 0;

						oh->shadows_obj = 0;
						oh->inband_shadowed_obj_id = 0;
						tags.extra_shadows = 0;

						/* Update file size */
						if(object->variant_type == YAFFS_OBJECT_TYPE_FILE){
							oh->file_size = object->variant.file_variant.file_size;
							tags.extra_length = oh->file_size;
						}

						yaffs_verify_oh(object, oh, &tags, 1);
						newChunk =
						    yaffs_write_new_chunk(dev,(__u8 *) oh, &tags, 1);
					} else
						newChunk =
						    yaffs_write_new_chunk(dev, buffer, &tags, 1);

					if (newChunk < 0) {
						retVal = YAFFS_FAIL;
					} else {

						/* Ok, now fix up the Tnodes etc. */

						if (tags.chunk_id == 0) {
							/* It's a header */
							object->hdr_chunk =  newChunk;
							object->serial =   tags.serial_number;
						} else {
							/* It's a data chunk */
							int ok;
							ok = yaffs_put_chunk_in_file
							    (object,
							     tags.chunk_id,
							     newChunk, 0);
						}
					}
				}

				if (retVal == YAFFS_OK)
					yaffs_chunk_del(dev, oldChunk, mark_flash, __LINE__);

			}
		}

		yaffs_release_temp_buffer(dev, buffer, __LINE__);



	}

	yaffs_verify_collected_blk(dev, bi, block);



	if (bi->block_state == YAFFS_BLOCK_STATE_COLLECTING) {
		/*
		 * The gc did not complete. Set block state back to FULL
		 * because checkpointing does not restore gc.
		 */
		bi->block_state = YAFFS_BLOCK_STATE_FULL;
	} else {
		/* The gc completed. */
		/* Do any required cleanups */
		for (i = 0; i < dev->n_clean_ups; i++) {
			/* Time to delete the file too */
			object =
			    yaffs_find_by_number(dev,
						     dev->gc_cleanup_list[i]);
			if (object) {
				yaffs_free_tnode(dev,
						object->variant.file_variant.
						top);
				object->variant.file_variant.top = NULL;
				T(YAFFS_TRACE_GC,
				  (TSTR
				   ("yaffs: About to finally delete object %d"
				    TENDSTR), object->obj_id));
				yaffs_generic_obj_del(object);
				object->my_dev->n_deleted_files--;
			}

		}


		chunksAfter = yaffs_get_erased_chunks(dev);
		if (chunksBefore >= chunksAfter) {
			T(YAFFS_TRACE_GC,
			  (TSTR
			   ("gc did not increase free chunks before %d after %d"
			    TENDSTR), chunksBefore, chunksAfter));
		}
		dev->gc_block = 0;
		dev->gc_chunk = 0;
		dev->n_clean_ups = 0;
	}

	dev->gc_disable = 0;

	return retVal;
}

/*
 * FindBlockForgarbageCollection is used to select the dirtiest block (or close enough)
 * for garbage collection.
 */

static unsigned yaffs_find_gc_block(yaffs_dev_t *dev,
					int aggressive,
					int background)
{
	int i;
	int iterations;
	unsigned selected = 0;
	int prioritised = 0;
	int prioritisedExists = 0;
	yaffs_block_info_t *bi;
	int threshold;

	/* First let's see if we need to grab a prioritised block */
	if (dev->has_pending_prioritised_gc && !aggressive) {
		dev->gc_dirtiest = 0;
		bi = dev->block_info;
		for (i = dev->internal_start_block;
			i <= dev->internal_end_block && !selected;
			i++) {

			if (bi->gc_prioritise) {
				prioritisedExists = 1;
				if (bi->block_state == YAFFS_BLOCK_STATE_FULL &&
				   yaffs_block_ok_for_gc(dev, bi)) {
					selected = i;
					prioritised = 1;
				}
			}
			bi++;
		}

		/*
		 * If there is a prioritised block and none was selected then
		 * this happened because there is at least one old dirty block gumming
		 * up the works. Let's gc the oldest dirty block.
		 */

		if(prioritisedExists &&
			!selected &&
			dev->oldest_dirty_block > 0)
			selected = dev->oldest_dirty_block;

		if (!prioritisedExists) /* None found, so we can clear this */
			dev->has_pending_prioritised_gc = 0;
	}

	/* If we're doing aggressive GC then we are happy to take a less-dirty block, and
	 * search harder.
	 * else (we're doing a leasurely gc), then we only bother to do this if the
	 * block has only a few pages in use.
	 */

	if (!selected){
		int pagesUsed;
		int nBlocks = dev->internal_end_block - dev->internal_start_block + 1;
		if (aggressive){
			threshold = dev->param.chunks_per_block;
			iterations = nBlocks;
		} else {
			int maxThreshold;

			if(background)
				maxThreshold = dev->param.chunks_per_block/2;
			else
				maxThreshold = dev->param.chunks_per_block/8;

			if(maxThreshold <  YAFFS_GC_PASSIVE_THRESHOLD)
				maxThreshold = YAFFS_GC_PASSIVE_THRESHOLD;

			threshold = background ?
				(dev->gc_not_done + 2) * 2 : 0;
			if(threshold <YAFFS_GC_PASSIVE_THRESHOLD)
				threshold = YAFFS_GC_PASSIVE_THRESHOLD;
			if(threshold > maxThreshold)
				threshold = maxThreshold;

			iterations = nBlocks / 16 + 1;
			if (iterations > 100)
				iterations = 100;
		}

		for (i = 0;
			i < iterations &&
			(dev->gc_dirtiest < 1 ||
				dev->gc_pages_in_use > YAFFS_GC_GOOD_ENOUGH);
			i++) {
			dev->gc_block_finder++;
			if (dev->gc_block_finder < dev->internal_start_block ||
				dev->gc_block_finder > dev->internal_end_block)
				dev->gc_block_finder = dev->internal_start_block;

			bi = yaffs_get_block_info(dev, dev->gc_block_finder);

			pagesUsed = bi->pages_in_use - bi->soft_del_pages;

			if (bi->block_state == YAFFS_BLOCK_STATE_FULL &&
				pagesUsed < dev->param.chunks_per_block &&
				(dev->gc_dirtiest < 1 || pagesUsed < dev->gc_pages_in_use) &&
				yaffs_block_ok_for_gc(dev, bi)) {
				dev->gc_dirtiest = dev->gc_block_finder;
				dev->gc_pages_in_use = pagesUsed;
			}
		}

		if(dev->gc_dirtiest > 0 && dev->gc_pages_in_use <= threshold)
			selected = dev->gc_dirtiest;
	}

	/*
	 * If nothing has been selected for a while, try selecting the oldest dirty
	 * because that's gumming up the works.
	 */

	if(!selected && dev->param.is_yaffs2 &&
		dev->gc_not_done >= ( background ? 10 : 20)){
		yaffs2_find_oldest_dirty_seq(dev);
		if(dev->oldest_dirty_block > 0) {
			selected = dev->oldest_dirty_block;
			dev->gc_dirtiest = selected;
			dev->oldest_dirty_gc_count++;
			bi = yaffs_get_block_info(dev, selected);
			dev->gc_pages_in_use =  bi->pages_in_use - bi->soft_del_pages;
		} else
			dev->gc_not_done = 0;
	}

	if(selected){
		T(YAFFS_TRACE_GC,
		  (TSTR("GC Selected block %d with %d free, prioritised:%d" TENDSTR),
		  selected,
		  dev->param.chunks_per_block - dev->gc_pages_in_use,
		  prioritised));

		dev->n_gc_blocks++;
		if(background)
			dev->bg_gcs++;

		dev->gc_dirtiest = 0;
		dev->gc_pages_in_use = 0;
		dev->gc_not_done = 0;
		if(dev->refresh_skip > 0)
			dev->refresh_skip--;
	} else{
		dev->gc_not_done++;
		T(YAFFS_TRACE_GC,
		  (TSTR("GC none: finder %d skip %d threshold %d dirtiest %d using %d oldest %d%s" TENDSTR),
		  dev->gc_block_finder, dev->gc_not_done,
		  threshold,
		  dev->gc_dirtiest, dev->gc_pages_in_use,
		  dev->oldest_dirty_block,
		  background ? " bg" : ""));
	}

	return selected;
}

/* New garbage collector
 * If we're very low on erased blocks then we do aggressive garbage collection
 * otherwise we do "leasurely" garbage collection.
 * Aggressive gc looks further (whole array) and will accept less dirty blocks.
 * Passive gc only inspects smaller areas and will only accept more dirty blocks.
 *
 * The idea is to help clear out space in a more spread-out manner.
 * Dunno if it really does anything useful.
 */
static int yaffs_check_gc(yaffs_dev_t *dev, int background)
{
	int aggressive = 0;
	int gcOk = YAFFS_OK;
	int maxTries = 0;
	int minErased;
	int erasedChunks;
	int checkpointBlockAdjust;

	if(dev->param.gc_control &&
		(dev->param.gc_control(dev) & 1) == 0)
		return YAFFS_OK;

	if (dev->gc_disable) {
		/* Bail out so we don't get recursive gc */
		return YAFFS_OK;
	}

	/* This loop should pass the first time.
	 * We'll only see looping here if the collection does not increase space.
	 */

	do {
		maxTries++;

		checkpointBlockAdjust = yaffs_calc_checkpt_blocks_required(dev);

		minErased  = dev->param.n_reserved_blocks + checkpointBlockAdjust + 1;
		erasedChunks = dev->n_erased_blocks * dev->param.chunks_per_block;

		/* If we need a block soon then do aggressive gc.*/
		if (dev->n_erased_blocks < minErased)
			aggressive = 1;
		else {
			if(!background && erasedChunks > (dev->n_free_chunks / 4))
				break;

			if(dev->gc_skip > 20)
				dev->gc_skip = 20;
			if(erasedChunks < dev->n_free_chunks/2 ||
				dev->gc_skip < 1 ||
				background)
				aggressive = 0;
			else {
				dev->gc_skip--;
				break;
			}
		}

		dev->gc_skip = 5;

                /* If we don't already have a block being gc'd then see if we should start another */

		if (dev->gc_block < 1 && !aggressive) {
			dev->gc_block = yaffs2_find_refresh_block(dev);
			dev->gc_chunk = 0;
			dev->n_clean_ups=0;
		}
		if (dev->gc_block < 1) {
			dev->gc_block = yaffs_find_gc_block(dev, aggressive, background);
			dev->gc_chunk = 0;
			dev->n_clean_ups=0;
		}

		if (dev->gc_block > 0) {
			dev->all_gcs++;
			if (!aggressive)
				dev->passive_gc_count++;

			T(YAFFS_TRACE_GC,
			  (TSTR
			   ("yaffs: GC erasedBlocks %d aggressive %d" TENDSTR),
			   dev->n_erased_blocks, aggressive));

			gcOk = yaffs_gc_block(dev, dev->gc_block, aggressive);
		}

		if (dev->n_erased_blocks < (dev->param.n_reserved_blocks) && dev->gc_block > 0) {
			T(YAFFS_TRACE_GC,
			  (TSTR
			   ("yaffs: GC !!!no reclaim!!! erasedBlocks %d after try %d block %d"
			    TENDSTR), dev->n_erased_blocks, maxTries, dev->gc_block));
		}
	} while ((dev->n_erased_blocks < dev->param.n_reserved_blocks) &&
		 (dev->gc_block > 0) &&
		 (maxTries < 2));

	return aggressive ? gcOk : YAFFS_OK;
}

/*
 * yaffs_bg_gc()
 * Garbage collects. Intended to be called from a background thread.
 * Returns non-zero if at least half the free chunks are erased.
 */
int yaffs_bg_gc(yaffs_dev_t *dev, unsigned urgency)
{
	int erasedChunks = dev->n_erased_blocks * dev->param.chunks_per_block;

	T(YAFFS_TRACE_BACKGROUND, (TSTR("Background gc %u" TENDSTR),urgency));

	yaffs_check_gc(dev, 1);
	return erasedChunks > dev->n_free_chunks/2;
}

/*-------------------------  TAGS --------------------------------*/

static int yaffs_tags_match(const yaffs_ext_tags *tags, int obj_id,
			   int chunkInObject)
{
	return (tags->chunk_id == chunkInObject &&
		tags->obj_id == obj_id && !tags->is_deleted) ? 1 : 0;

}


/*-------------------- Data file manipulation -----------------*/

static int yaffs_find_chunk_in_file(yaffs_obj_t *in, int inode_chunk,
				 yaffs_ext_tags *tags)
{
	/*Get the Tnode, then get the level 0 offset chunk offset */
	yaffs_tnode_t *tn;
	int theChunk = -1;
	yaffs_ext_tags localTags;
	int retVal = -1;

	yaffs_dev_t *dev = in->my_dev;

	if (!tags) {
		/* Passed a NULL, so use our own tags space */
		tags = &localTags;
	}

	tn = yaffs_find_tnode_0(dev, &in->variant.file_variant, inode_chunk);

	if (tn) {
		theChunk = yaffs_get_group_base(dev, tn, inode_chunk);

		retVal =
		    yaffs_find_chunk_in_group(dev, theChunk, tags, in->obj_id,
					   inode_chunk);
	}
	return retVal;
}

static int yaffs_find_del_file_chunk(yaffs_obj_t *in, int inode_chunk,
					  yaffs_ext_tags *tags)
{
	/* Get the Tnode, then get the level 0 offset chunk offset */
	yaffs_tnode_t *tn;
	int theChunk = -1;
	yaffs_ext_tags localTags;

	yaffs_dev_t *dev = in->my_dev;
	int retVal = -1;

	if (!tags) {
		/* Passed a NULL, so use our own tags space */
		tags = &localTags;
	}

	tn = yaffs_find_tnode_0(dev, &in->variant.file_variant, inode_chunk);

	if (tn) {

		theChunk = yaffs_get_group_base(dev, tn, inode_chunk);

		retVal =
		    yaffs_find_chunk_in_group(dev, theChunk, tags, in->obj_id,
					   inode_chunk);

		/* Delete the entry in the filestructure (if found) */
		if (retVal != -1)
			yaffs_load_tnode_0(dev, tn, inode_chunk, 0);
	}

	return retVal;
}


int yaffs_put_chunk_in_file(yaffs_obj_t *in, int inode_chunk,
			        int nand_chunk, int in_scan)
{
	/* NB in_scan is zero unless scanning.
	 * For forward scanning, in_scan is > 0;
	 * for backward scanning in_scan is < 0
	 *
	 * nand_chunk = 0 is a dummy insert to make sure the tnodes are there.
	 */

	yaffs_tnode_t *tn;
	yaffs_dev_t *dev = in->my_dev;
	int existingChunk;
	yaffs_ext_tags existingTags;
	yaffs_ext_tags newTags;
	unsigned existingSerial, newSerial;

	if (in->variant_type != YAFFS_OBJECT_TYPE_FILE) {
		/* Just ignore an attempt at putting a chunk into a non-file during scanning
		 * If it is not during Scanning then something went wrong!
		 */
		if (!in_scan) {
			T(YAFFS_TRACE_ERROR,
			  (TSTR
			   ("yaffs tragedy:attempt to put data chunk into a non-file"
			    TENDSTR)));
			YBUG();
		}

		yaffs_chunk_del(dev, nand_chunk, 1, __LINE__);
		return YAFFS_OK;
	}

	tn = yaffs_add_find_tnode_0(dev,
					&in->variant.file_variant,
					inode_chunk,
					NULL);
	if (!tn)
		return YAFFS_FAIL;
	
	if(!nand_chunk)
		/* Dummy insert, bail now */
		return YAFFS_OK;

	existingChunk = yaffs_get_group_base(dev, tn, inode_chunk);

	if (in_scan != 0) {
		/* If we're scanning then we need to test for duplicates
		 * NB This does not need to be efficient since it should only ever
		 * happen when the power fails during a write, then only one
		 * chunk should ever be affected.
		 *
		 * Correction for YAFFS2: This could happen quite a lot and we need to think about efficiency! TODO
		 * Update: For backward scanning we don't need to re-read tags so this is quite cheap.
		 */

		if (existingChunk > 0) {
			/* NB Right now existing chunk will not be real chunk_id if the chunk group size > 1
			 *    thus we have to do a FindChunkInFile to get the real chunk id.
			 *
			 * We have a duplicate now we need to decide which one to use:
			 *
			 * Backwards scanning YAFFS2: The old one is what we use, dump the new one.
			 * Forward scanning YAFFS2: The new one is what we use, dump the old one.
			 * YAFFS1: Get both sets of tags and compare serial numbers.
			 */

			if (in_scan > 0) {
				/* Only do this for forward scanning */
				yaffs_rd_chunk_tags_nand(dev,
								nand_chunk,
								NULL, &newTags);

				/* Do a proper find */
				existingChunk =
				    yaffs_find_chunk_in_file(in, inode_chunk,
							  &existingTags);
			}

			if (existingChunk <= 0) {
				/*Hoosterman - how did this happen? */

				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("yaffs tragedy: existing chunk < 0 in scan"
				    TENDSTR)));

			}

			/* NB The deleted flags should be false, otherwise the chunks will
			 * not be loaded during a scan
			 */

			if (in_scan > 0) {
				newSerial = newTags.serial_number;
				existingSerial = existingTags.serial_number;
			}

			if ((in_scan > 0) &&
			    (existingChunk <= 0 ||
			     ((existingSerial + 1) & 3) == newSerial)) {
				/* Forward scanning.
				 * Use new
				 * Delete the old one and drop through to update the tnode
				 */
				yaffs_chunk_del(dev, existingChunk, 1,
						  __LINE__);
			} else {
				/* Backward scanning or we want to use the existing one
				 * Use existing.
				 * Delete the new one and return early so that the tnode isn't changed
				 */
				yaffs_chunk_del(dev, nand_chunk, 1,
						  __LINE__);
				return YAFFS_OK;
			}
		}

	}

	if (existingChunk == 0)
		in->n_data_chunks++;

	yaffs_load_tnode_0(dev, tn, inode_chunk, nand_chunk);

	return YAFFS_OK;
}

static int yaffs_rd_data_obj(yaffs_obj_t *in, int inode_chunk,
					__u8 *buffer)
{
	int nand_chunk = yaffs_find_chunk_in_file(in, inode_chunk, NULL);

	if (nand_chunk >= 0)
		return yaffs_rd_chunk_tags_nand(in->my_dev, nand_chunk,
						buffer, NULL);
	else {
		T(YAFFS_TRACE_NANDACCESS,
		  (TSTR("Chunk %d not found zero instead" TENDSTR),
		   nand_chunk));
		/* get sane (zero) data if you read a hole */
		memset(buffer, 0, in->my_dev->data_bytes_per_chunk);
		return 0;
	}

}

void yaffs_chunk_del(yaffs_dev_t *dev, int chunk_id, int mark_flash, int lyn)
{
	int block;
	int page;
	yaffs_ext_tags tags;
	yaffs_block_info_t *bi;

	if (chunk_id <= 0)
		return;

	dev->n_deletions++;
	block = chunk_id / dev->param.chunks_per_block;
	page = chunk_id % dev->param.chunks_per_block;


	if (!yaffs_check_chunk_bit(dev, block, page))
		T(YAFFS_TRACE_VERIFY,
			(TSTR("Deleting invalid chunk %d"TENDSTR),
			 chunk_id));

	bi = yaffs_get_block_info(dev, block);
	
	yaffs2_update_oldest_dirty_seq(dev, block, bi);

	T(YAFFS_TRACE_DELETION,
	  (TSTR("line %d delete of chunk %d" TENDSTR), lyn, chunk_id));

	if (!dev->param.is_yaffs2 && mark_flash &&
	    bi->block_state != YAFFS_BLOCK_STATE_COLLECTING) {

		yaffs_init_tags(&tags);

		tags.is_deleted = 1;

		yaffs_wr_chunk_tags_nand(dev, chunk_id, NULL, &tags);
		yaffs_handle_chunk_update(dev, chunk_id, &tags);
	} else {
		dev->n_unmarked_deletions++;
	}

	/* Pull out of the management area.
	 * If the whole block became dirty, this will kick off an erasure.
	 */
	if (bi->block_state == YAFFS_BLOCK_STATE_ALLOCATING ||
	    bi->block_state == YAFFS_BLOCK_STATE_FULL ||
	    bi->block_state == YAFFS_BLOCK_STATE_NEEDS_SCANNING ||
	    bi->block_state == YAFFS_BLOCK_STATE_COLLECTING) {
		dev->n_free_chunks++;

		yaffs_clear_chunk_bit(dev, block, page);

		bi->pages_in_use--;

		if (bi->pages_in_use == 0 &&
		    !bi->has_shrink_hdr &&
		    bi->block_state != YAFFS_BLOCK_STATE_ALLOCATING &&
		    bi->block_state != YAFFS_BLOCK_STATE_NEEDS_SCANNING) {
			yaffs_block_became_dirty(dev, block);
		}

	}

}

static int yaffs_wr_data_obj(yaffs_obj_t *in, int inode_chunk,
					const __u8 *buffer, int n_bytes,
					int useReserve)
{
	/* Find old chunk Need to do this to get serial number
	 * Write new one and patch into tree.
	 * Invalidate old tags.
	 */

	int prevChunkId;
	yaffs_ext_tags prevTags;

	int newChunkId;
	yaffs_ext_tags newTags;

	yaffs_dev_t *dev = in->my_dev;

	yaffs_check_gc(dev,0);

	/* Get the previous chunk at this location in the file if it exists.
	 * If it does not exist then put a zero into the tree. This creates
	 * the tnode now, rather than later when it is harder to clean up.
	 */
	prevChunkId = yaffs_find_chunk_in_file(in, inode_chunk, &prevTags);
	if(prevChunkId < 1 &&
		!yaffs_put_chunk_in_file(in, inode_chunk, 0, 0))
		return 0;

	/* Set up new tags */
	yaffs_init_tags(&newTags);

	newTags.chunk_id = inode_chunk;
	newTags.obj_id = in->obj_id;
	newTags.serial_number =
	    (prevChunkId > 0) ? prevTags.serial_number + 1 : 1;
	newTags.n_bytes = n_bytes;

	if (n_bytes < 1 || n_bytes > dev->param.total_bytes_per_chunk) {
		T(YAFFS_TRACE_ERROR,
		(TSTR("Writing %d bytes to chunk!!!!!!!!!" TENDSTR), n_bytes));
		YBUG();
	}
	
		
	newChunkId =
	    yaffs_write_new_chunk(dev, buffer, &newTags,
					      useReserve);

	if (newChunkId > 0) {
		yaffs_put_chunk_in_file(in, inode_chunk, newChunkId, 0);

		if (prevChunkId > 0)
			yaffs_chunk_del(dev, prevChunkId, 1, __LINE__);

		yaffs_verify_file_sane(in);
	}
	return newChunkId;

}

/* UpdateObjectHeader updates the header on NAND for an object.
 * If name is not NULL, then that new name is used.
 */
int yaffs_update_oh(yaffs_obj_t *in, const YCHAR *name, int force,
			     int is_shrink, int shadows, yaffs_xattr_mod *xmod)
{

	yaffs_block_info_t *bi;

	yaffs_dev_t *dev = in->my_dev;

	int prevChunkId;
	int retVal = 0;
	int result = 0;

	int newChunkId;
	yaffs_ext_tags newTags;
	yaffs_ext_tags oldTags;
	const YCHAR *alias = NULL;

	__u8 *buffer = NULL;
	YCHAR old_name[YAFFS_MAX_NAME_LENGTH + 1];

	yaffs_obj_header *oh = NULL;

	yaffs_strcpy(old_name, _Y("silly old name"));


	if (!in->fake ||
		in == dev->root_dir || /* The root_dir should also be saved */
		force  || xmod) {

		yaffs_check_gc(dev,0);
		yaffs_check_obj_details_loaded(in);

		buffer = yaffs_get_temp_buffer(in->my_dev, __LINE__);
		oh = (yaffs_obj_header *) buffer;

		prevChunkId = in->hdr_chunk;

		if (prevChunkId > 0) {
			result = yaffs_rd_chunk_tags_nand(dev, prevChunkId,
							buffer, &oldTags);

			yaffs_verify_oh(in, oh, &oldTags, 0);

			memcpy(old_name, oh->name, sizeof(oh->name));
			memset(buffer, 0xFF, sizeof(yaffs_obj_header));
		} else
			memset(buffer, 0xFF, dev->data_bytes_per_chunk);

		oh->type = in->variant_type;
		oh->yst_mode = in->yst_mode;
		oh->shadows_obj = oh->inband_shadowed_obj_id = shadows;

#ifdef CONFIG_YAFFS_WINCE
		oh->win_atime[0] = in->win_atime[0];
		oh->win_ctime[0] = in->win_ctime[0];
		oh->win_mtime[0] = in->win_mtime[0];
		oh->win_atime[1] = in->win_atime[1];
		oh->win_ctime[1] = in->win_ctime[1];
		oh->win_mtime[1] = in->win_mtime[1];
#else
		oh->yst_uid = in->yst_uid;
		oh->yst_gid = in->yst_gid;
		oh->yst_atime = in->yst_atime;
		oh->yst_mtime = in->yst_mtime;
		oh->yst_ctime = in->yst_ctime;
		oh->yst_rdev = in->yst_rdev;
#endif
		if (in->parent)
			oh->parent_obj_id = in->parent->obj_id;
		else
			oh->parent_obj_id = 0;

		if (name && *name) {
			memset(oh->name, 0, sizeof(oh->name));
			yaffs_load_oh_from_name(dev,oh->name,name);
		} else if (prevChunkId > 0)
			memcpy(oh->name, old_name, sizeof(oh->name));
		else
			memset(oh->name, 0, sizeof(oh->name));

		oh->is_shrink = is_shrink;

		switch (in->variant_type) {
		case YAFFS_OBJECT_TYPE_UNKNOWN:
			/* Should not happen */
			break;
		case YAFFS_OBJECT_TYPE_FILE:
			oh->file_size =
			    (oh->parent_obj_id == YAFFS_OBJECTID_DELETED
			     || oh->parent_obj_id ==
			     YAFFS_OBJECTID_UNLINKED) ? 0 : in->variant.
			    file_variant.file_size;
			break;
		case YAFFS_OBJECT_TYPE_HARDLINK:
			oh->equiv_id =
			    in->variant.hardlink_variant.equiv_id;
			break;
		case YAFFS_OBJECT_TYPE_SPECIAL:
			/* Do nothing */
			break;
		case YAFFS_OBJECT_TYPE_DIRECTORY:
			/* Do nothing */
			break;
		case YAFFS_OBJECT_TYPE_SYMLINK:
			alias = in->variant.symlink_variant.alias;
			if(!alias)
				alias = _Y("no alias");
			yaffs_strncpy(oh->alias,
					alias,
				      YAFFS_MAX_ALIAS_LENGTH);
			oh->alias[YAFFS_MAX_ALIAS_LENGTH] = 0;
			break;
		}

		/* process any xattrib modifications */
		if(xmod)
			yaffs_apply_xattrib_mod(in, (char *)buffer, xmod);


		/* Tags */
		yaffs_init_tags(&newTags);
		in->serial++;
		newTags.chunk_id = 0;
		newTags.obj_id = in->obj_id;
		newTags.serial_number = in->serial;

		/* Add extra info for file header */

		newTags.extra_available = 1;
		newTags.extra_parent_id = oh->parent_obj_id;
		newTags.extra_length = oh->file_size;
		newTags.extra_is_shrink = oh->is_shrink;
		newTags.extra_equiv_id = oh->equiv_id;
		newTags.extra_shadows = (oh->shadows_obj > 0) ? 1 : 0;
		newTags.extra_obj_type = in->variant_type;

		yaffs_verify_oh(in, oh, &newTags, 1);

		/* Create new chunk in NAND */
		newChunkId =
		    yaffs_write_new_chunk(dev, buffer, &newTags,
						      (prevChunkId > 0) ? 1 : 0);

		if (newChunkId >= 0) {

			in->hdr_chunk = newChunkId;

			if (prevChunkId > 0) {
				yaffs_chunk_del(dev, prevChunkId, 1,
						  __LINE__);
			}

			if (!yaffs_obj_cache_dirty(in))
				in->dirty = 0;

			/* If this was a shrink, then mark the block that the chunk lives on */
			if (is_shrink) {
				bi = yaffs_get_block_info(in->my_dev,
					newChunkId / in->my_dev->param.chunks_per_block);
				bi->has_shrink_hdr = 1;
			}

		}

		retVal = newChunkId;

	}

	if (buffer)
		yaffs_release_temp_buffer(dev, buffer, __LINE__);

	return retVal;
}

/*------------------------ Short Operations Cache ----------------------------------------
 *   In many situations where there is no high level buffering (eg WinCE) a lot of
 *   reads might be short sequential reads, and a lot of writes may be short
 *   sequential writes. eg. scanning/writing a jpeg file.
 *   In these cases, a short read/write cache can provide a huge perfomance benefit
 *   with dumb-as-a-rock code.
 *   In Linux, the page cache provides read buffering aand the short op cache provides write
 *   buffering.
 *
 *   There are a limited number (~10) of cache chunks per device so that we don't
 *   need a very intelligent search.
 */

static int yaffs_obj_cache_dirty(yaffs_obj_t *obj)
{
	yaffs_dev_t *dev = obj->my_dev;
	int i;
	yaffs_cache_t *cache;
	int nCaches = obj->my_dev->param.n_caches;

	for (i = 0; i < nCaches; i++) {
		cache = &dev->cache[i];
		if (cache->object == obj &&
		    cache->dirty)
			return 1;
	}

	return 0;
}


static void yaffs_flush_file_cache(yaffs_obj_t *obj)
{
	yaffs_dev_t *dev = obj->my_dev;
	int lowest = -99;	/* Stop compiler whining. */
	int i;
	yaffs_cache_t *cache;
	int chunkWritten = 0;
	int nCaches = obj->my_dev->param.n_caches;

	if (nCaches > 0) {
		do {
			cache = NULL;

			/* Find the dirty cache for this object with the lowest chunk id. */
			for (i = 0; i < nCaches; i++) {
				if (dev->cache[i].object == obj &&
				    dev->cache[i].dirty) {
					if (!cache
					    || dev->cache[i].chunk_id <
					    lowest) {
						cache = &dev->cache[i];
						lowest = cache->chunk_id;
					}
				}
			}

			if (cache && !cache->locked) {
				/* Write it out and free it up */

				chunkWritten =
				    yaffs_wr_data_obj(cache->object,
								 cache->chunk_id,
								 cache->data,
								 cache->n_bytes,
								 1);
				cache->dirty = 0;
				cache->object = NULL;
			}

		} while (cache && chunkWritten > 0);

		if (cache) {
			/* Hoosterman, disk full while writing cache out. */
			T(YAFFS_TRACE_ERROR,
			  (TSTR("yaffs tragedy: no space during cache write" TENDSTR)));

		}
	}

}

/*yaffs_flush_whole_cache(dev)
 *
 *
 */

void yaffs_flush_whole_cache(yaffs_dev_t *dev)
{
	yaffs_obj_t *obj;
	int nCaches = dev->param.n_caches;
	int i;

	/* Find a dirty object in the cache and flush it...
	 * until there are no further dirty objects.
	 */
	do {
		obj = NULL;
		for (i = 0; i < nCaches && !obj; i++) {
			if (dev->cache[i].object &&
			    dev->cache[i].dirty)
				obj = dev->cache[i].object;

		}
		if (obj)
			yaffs_flush_file_cache(obj);

	} while (obj);

}


/* Grab us a cache chunk for use.
 * First look for an empty one.
 * Then look for the least recently used non-dirty one.
 * Then look for the least recently used dirty one...., flush and look again.
 */
static yaffs_cache_t *yaffs_grab_chunk_worker(yaffs_dev_t *dev)
{
	int i;

	if (dev->param.n_caches > 0) {
		for (i = 0; i < dev->param.n_caches; i++) {
			if (!dev->cache[i].object)
				return &dev->cache[i];
		}
	}

	return NULL;
}

static yaffs_cache_t *yaffs_grab_chunk_cache(yaffs_dev_t *dev)
{
	yaffs_cache_t *cache;
	yaffs_obj_t *theObj;
	int usage;
	int i;
	int pushout;

	if (dev->param.n_caches > 0) {
		/* Try find a non-dirty one... */

		cache = yaffs_grab_chunk_worker(dev);

		if (!cache) {
			/* They were all dirty, find the last recently used object and flush
			 * its cache, then  find again.
			 * NB what's here is not very accurate, we actually flush the object
			 * the last recently used page.
			 */

			/* With locking we can't assume we can use entry zero */

			theObj = NULL;
			usage = -1;
			cache = NULL;
			pushout = -1;

			for (i = 0; i < dev->param.n_caches; i++) {
				if (dev->cache[i].object &&
				    !dev->cache[i].locked &&
				    (dev->cache[i].last_use < usage || !cache)) {
					usage = dev->cache[i].last_use;
					theObj = dev->cache[i].object;
					cache = &dev->cache[i];
					pushout = i;
				}
			}

			if (!cache || cache->dirty) {
				/* Flush and try again */
				yaffs_flush_file_cache(theObj);
				cache = yaffs_grab_chunk_worker(dev);
			}

		}
		return cache;
	} else
		return NULL;

}

/* Find a cached chunk */
static yaffs_cache_t *yaffs_find_chunk_cache(const yaffs_obj_t *obj,
					      int chunk_id)
{
	yaffs_dev_t *dev = obj->my_dev;
	int i;
	if (dev->param.n_caches > 0) {
		for (i = 0; i < dev->param.n_caches; i++) {
			if (dev->cache[i].object == obj &&
			    dev->cache[i].chunk_id == chunk_id) {
				dev->cache_hits++;

				return &dev->cache[i];
			}
		}
	}
	return NULL;
}

/* Mark the chunk for the least recently used algorithym */
static void yaffs_use_cache(yaffs_dev_t *dev, yaffs_cache_t *cache,
				int isAWrite)
{

	if (dev->param.n_caches > 0) {
		if (dev->cache_last_use < 0 || dev->cache_last_use > 100000000) {
			/* Reset the cache usages */
			int i;
			for (i = 1; i < dev->param.n_caches; i++)
				dev->cache[i].last_use = 0;

			dev->cache_last_use = 0;
		}

		dev->cache_last_use++;

		cache->last_use = dev->cache_last_use;

		if (isAWrite)
			cache->dirty = 1;
	}
}

/* Invalidate a single cache page.
 * Do this when a whole page gets written,
 * ie the short cache for this page is no longer valid.
 */
static void yaffs_invalidate_chunk_cache(yaffs_obj_t *object, int chunk_id)
{
	if (object->my_dev->param.n_caches > 0) {
		yaffs_cache_t *cache = yaffs_find_chunk_cache(object, chunk_id);

		if (cache)
			cache->object = NULL;
	}
}

/* Invalidate all the cache pages associated with this object
 * Do this whenever ther file is deleted or resized.
 */
static void yaffs_invalidate_whole_cache(yaffs_obj_t *in)
{
	int i;
	yaffs_dev_t *dev = in->my_dev;

	if (dev->param.n_caches > 0) {
		/* Invalidate it. */
		for (i = 0; i < dev->param.n_caches; i++) {
			if (dev->cache[i].object == in)
				dev->cache[i].object = NULL;
		}
	}
}


/*--------------------- File read/write ------------------------
 * Read and write have very similar structures.
 * In general the read/write has three parts to it
 * An incomplete chunk to start with (if the read/write is not chunk-aligned)
 * Some complete chunks
 * An incomplete chunk to end off with
 *
 * Curve-balls: the first chunk might also be the last chunk.
 */

int yaffs_file_rd(yaffs_obj_t *in, __u8 *buffer, loff_t offset,
			int n_bytes)
{

	int chunk;
	__u32 start;
	int nToCopy;
	int n = n_bytes;
	int nDone = 0;
	yaffs_cache_t *cache;

	yaffs_dev_t *dev;

	dev = in->my_dev;

	while (n > 0) {
		/* chunk = offset / dev->data_bytes_per_chunk + 1; */
		/* start = offset % dev->data_bytes_per_chunk; */
		yaffs_addr_to_chunk(dev, offset, &chunk, &start);
		chunk++;

		/* OK now check for the curveball where the start and end are in
		 * the same chunk.
		 */
		if ((start + n) < dev->data_bytes_per_chunk)
			nToCopy = n;
		else
			nToCopy = dev->data_bytes_per_chunk - start;

		cache = yaffs_find_chunk_cache(in, chunk);

		/* If the chunk is already in the cache or it is less than a whole chunk
		 * or we're using inband tags then use the cache (if there is caching)
		 * else bypass the cache.
		 */
		if (cache || nToCopy != dev->data_bytes_per_chunk || dev->param.inband_tags) {
			if (dev->param.n_caches > 0) {

				/* If we can't find the data in the cache, then load it up. */

				if (!cache) {
					cache = yaffs_grab_chunk_cache(in->my_dev);
					cache->object = in;
					cache->chunk_id = chunk;
					cache->dirty = 0;
					cache->locked = 0;
					yaffs_rd_data_obj(in, chunk,
								      cache->
								      data);
					cache->n_bytes = 0;
				}

				yaffs_use_cache(dev, cache, 0);

				cache->locked = 1;


				memcpy(buffer, &cache->data[start], nToCopy);

				cache->locked = 0;
			} else {
				/* Read into the local buffer then copy..*/

				__u8 *localBuffer =
				    yaffs_get_temp_buffer(dev, __LINE__);
				yaffs_rd_data_obj(in, chunk,
							      localBuffer);

				memcpy(buffer, &localBuffer[start], nToCopy);


				yaffs_release_temp_buffer(dev, localBuffer,
							__LINE__);
			}

		} else {

			/* A full chunk. Read directly into the supplied buffer. */
			yaffs_rd_data_obj(in, chunk, buffer);

		}

		n -= nToCopy;
		offset += nToCopy;
		buffer += nToCopy;
		nDone += nToCopy;

	}

	return nDone;
}

int yaffs_do_file_wr(yaffs_obj_t *in, const __u8 *buffer, loff_t offset,
			int n_bytes, int write_trhrough)
{

	int chunk;
	__u32 start;
	int nToCopy;
	int n = n_bytes;
	int nDone = 0;
	int nToWriteBack;
	int startOfWrite = offset;
	int chunkWritten = 0;
	__u32 n_bytesRead;
	__u32 chunkStart;

	yaffs_dev_t *dev;

	dev = in->my_dev;

	while (n > 0 && chunkWritten >= 0) {
		yaffs_addr_to_chunk(dev, offset, &chunk, &start);

		if (chunk * dev->data_bytes_per_chunk + start != offset ||
				start >= dev->data_bytes_per_chunk) {
			T(YAFFS_TRACE_ERROR, (
			   TSTR("AddrToChunk of offset %d gives chunk %d start %d"
			   TENDSTR),
			   (int)offset, chunk, start));
		}
		chunk++; /* File pos to chunk in file offset */

		/* OK now check for the curveball where the start and end are in
		 * the same chunk.
		 */

		if ((start + n) < dev->data_bytes_per_chunk) {
			nToCopy = n;

			/* Now folks, to calculate how many bytes to write back....
			 * If we're overwriting and not writing to then end of file then
			 * we need to write back as much as was there before.
			 */

			chunkStart = ((chunk - 1) * dev->data_bytes_per_chunk);

			if (chunkStart > in->variant.file_variant.file_size)
				n_bytesRead = 0; /* Past end of file */
			else
				n_bytesRead = in->variant.file_variant.file_size - chunkStart;

			if (n_bytesRead > dev->data_bytes_per_chunk)
				n_bytesRead = dev->data_bytes_per_chunk;

			nToWriteBack =
			    (n_bytesRead >
			     (start + n)) ? n_bytesRead : (start + n);

			if (nToWriteBack < 0 || nToWriteBack > dev->data_bytes_per_chunk)
				YBUG();

		} else {
			nToCopy = dev->data_bytes_per_chunk - start;
			nToWriteBack = dev->data_bytes_per_chunk;
		}

		if (nToCopy != dev->data_bytes_per_chunk || dev->param.inband_tags) {
			/* An incomplete start or end chunk (or maybe both start and end chunk),
			 * or we're using inband tags, so we want to use the cache buffers.
			 */
			if (dev->param.n_caches > 0) {
				yaffs_cache_t *cache;
				/* If we can't find the data in the cache, then load the cache */
				cache = yaffs_find_chunk_cache(in, chunk);

				if (!cache
				    && yaffs_check_alloc_available(dev, 1)) {
					cache = yaffs_grab_chunk_cache(dev);
					cache->object = in;
					cache->chunk_id = chunk;
					cache->dirty = 0;
					cache->locked = 0;
					yaffs_rd_data_obj(in, chunk,
								      cache->data);
				} else if (cache &&
					!cache->dirty &&
					!yaffs_check_alloc_available(dev, 1)) {
					/* Drop the cache if it was a read cache item and
					 * no space check has been made for it.
					 */
					 cache = NULL;
				}

				if (cache) {
					yaffs_use_cache(dev, cache, 1);
					cache->locked = 1;


					memcpy(&cache->data[start], buffer,
					       nToCopy);


					cache->locked = 0;
					cache->n_bytes = nToWriteBack;

					if (write_trhrough) {
						chunkWritten =
						    yaffs_wr_data_obj
						    (cache->object,
						     cache->chunk_id,
						     cache->data, cache->n_bytes,
						     1);
						cache->dirty = 0;
					}

				} else {
					chunkWritten = -1;	/* fail the write */
				}
			} else {
				/* An incomplete start or end chunk (or maybe both start and end chunk)
				 * Read into the local buffer then copy, then copy over and write back.
				 */

				__u8 *localBuffer =
				    yaffs_get_temp_buffer(dev, __LINE__);

				yaffs_rd_data_obj(in, chunk,
							      localBuffer);



				memcpy(&localBuffer[start], buffer, nToCopy);

				chunkWritten =
				    yaffs_wr_data_obj(in, chunk,
								 localBuffer,
								 nToWriteBack,
								 0);

				yaffs_release_temp_buffer(dev, localBuffer,
							__LINE__);

			}

		} else {
			/* A full chunk. Write directly from the supplied buffer. */



			chunkWritten =
			    yaffs_wr_data_obj(in, chunk, buffer,
							 dev->data_bytes_per_chunk,
							 0);

			/* Since we've overwritten the cached data, we better invalidate it. */
			yaffs_invalidate_chunk_cache(in, chunk);
		}

		if (chunkWritten >= 0) {
			n -= nToCopy;
			offset += nToCopy;
			buffer += nToCopy;
			nDone += nToCopy;
		}

	}

	/* Update file object */

	if ((startOfWrite + nDone) > in->variant.file_variant.file_size)
		in->variant.file_variant.file_size = (startOfWrite + nDone);

	in->dirty = 1;

	return nDone;
}

int yaffs_wr_file(yaffs_obj_t *in, const __u8 *buffer, loff_t offset,
			int n_bytes, int write_trhrough)
{
	yaffs2_handle_hole(in,offset);
	return yaffs_do_file_wr(in,buffer,offset,n_bytes,write_trhrough);
}



/* ---------------------- File resizing stuff ------------------ */

static void yaffs_prune_chunks(yaffs_obj_t *in, int new_size)
{

	yaffs_dev_t *dev = in->my_dev;
	int oldFileSize = in->variant.file_variant.file_size;

	int lastDel = 1 + (oldFileSize - 1) / dev->data_bytes_per_chunk;

	int startDel = 1 + (new_size + dev->data_bytes_per_chunk - 1) /
	    dev->data_bytes_per_chunk;
	int i;
	int chunk_id;

	/* Delete backwards so that we don't end up with holes if
	 * power is lost part-way through the operation.
	 */
	for (i = lastDel; i >= startDel; i--) {
		/* NB this could be optimised somewhat,
		 * eg. could retrieve the tags and write them without
		 * using yaffs_chunk_del
		 */

		chunk_id = yaffs_find_del_file_chunk(in, i, NULL);
		if (chunk_id > 0) {
			if (chunk_id <
			    (dev->internal_start_block * dev->param.chunks_per_block)
			    || chunk_id >=
			    ((dev->internal_end_block +
			      1) * dev->param.chunks_per_block)) {
				T(YAFFS_TRACE_ALWAYS,
				  (TSTR("Found daft chunk_id %d for %d" TENDSTR),
				   chunk_id, i));
			} else {
				in->n_data_chunks--;
				yaffs_chunk_del(dev, chunk_id, 1, __LINE__);
			}
		}
	}

}


void yaffs_resize_file_down( yaffs_obj_t *obj, loff_t new_size)
{
	int newFullChunks;
	__u32 new_sizeOfPartialChunk;
	yaffs_dev_t *dev = obj->my_dev;

	yaffs_addr_to_chunk(dev, new_size, &newFullChunks, &new_sizeOfPartialChunk);

	yaffs_prune_chunks(obj, new_size);

	if (new_sizeOfPartialChunk != 0) {
		int lastChunk = 1 + newFullChunks;
		__u8 *localBuffer = yaffs_get_temp_buffer(dev, __LINE__);

		/* Got to read and rewrite the last chunk with its new size and zero pad */
		yaffs_rd_data_obj(obj, lastChunk, localBuffer);
		memset(localBuffer + new_sizeOfPartialChunk, 0,
			dev->data_bytes_per_chunk - new_sizeOfPartialChunk);

		yaffs_wr_data_obj(obj, lastChunk, localBuffer,
					     new_sizeOfPartialChunk, 1);

		yaffs_release_temp_buffer(dev, localBuffer, __LINE__);
	}

	obj->variant.file_variant.file_size = new_size;

	yaffs_prune_tree(dev, &obj->variant.file_variant);
}


int yaffs_resize_file(yaffs_obj_t *in, loff_t new_size)
{
	yaffs_dev_t *dev = in->my_dev;
	int oldFileSize = in->variant.file_variant.file_size;

	yaffs_flush_file_cache(in);
	yaffs_invalidate_whole_cache(in);

	yaffs_check_gc(dev,0);

	if (in->variant_type != YAFFS_OBJECT_TYPE_FILE)
		return YAFFS_FAIL;

	if (new_size == oldFileSize)
		return YAFFS_OK;
		
	if(new_size > oldFileSize){
		yaffs2_handle_hole(in,new_size);
		in->variant.file_variant.file_size = new_size;
	} else {
		/* new_size < oldFileSize */ 
		yaffs_resize_file_down(in, new_size);
	} 

	/* Write a new object header to reflect the resize.
	 * show we've shrunk the file, if need be
	 * Do this only if the file is not in the deleted directories
	 * and is not shadowed.
	 */
	if (in->parent &&
	    !in->is_shadowed &&
	    in->parent->obj_id != YAFFS_OBJECTID_UNLINKED &&
	    in->parent->obj_id != YAFFS_OBJECTID_DELETED)
		yaffs_update_oh(in, NULL, 0, 0, 0, NULL);


	return YAFFS_OK;
}

loff_t yaffs_get_file_size(yaffs_obj_t *obj)
{
	YCHAR *alias = NULL;
	obj = yaffs_get_equivalent_obj(obj);

	switch (obj->variant_type) {
	case YAFFS_OBJECT_TYPE_FILE:
		return obj->variant.file_variant.file_size;
	case YAFFS_OBJECT_TYPE_SYMLINK:
		alias = obj->variant.symlink_variant.alias;
		if(!alias)
			return 0;
		return yaffs_strnlen(alias,YAFFS_MAX_ALIAS_LENGTH);
	default:
		return 0;
	}
}



int yaffs_flush_file(yaffs_obj_t *in, int update_time, int data_sync)
{
	int retVal;
	if (in->dirty) {
		yaffs_flush_file_cache(in);
		if(data_sync) /* Only sync data */
			retVal=YAFFS_OK;
		else {
			if (update_time) {
#ifdef CONFIG_YAFFS_WINCE
				yfsd_win_file_time_now(in->win_mtime);
#else

				in->yst_mtime = Y_CURRENT_TIME;

#endif
			}

			retVal = (yaffs_update_oh(in, NULL, 0, 0, 0, NULL) >=
				0) ? YAFFS_OK : YAFFS_FAIL;
		}
	} else {
		retVal = YAFFS_OK;
	}

	return retVal;

}

static int yaffs_generic_obj_del(yaffs_obj_t *in)
{

	/* First off, invalidate the file's data in the cache, without flushing. */
	yaffs_invalidate_whole_cache(in);

	if (in->my_dev->param.is_yaffs2 && (in->parent != in->my_dev->del_dir)) {
		/* Move to the unlinked directory so we have a record that it was deleted. */
		yaffs_change_obj_name(in, in->my_dev->del_dir, _Y("deleted"), 0, 0);

	}

	yaffs_remove_obj_from_dir(in);
	yaffs_chunk_del(in->my_dev, in->hdr_chunk, 1, __LINE__);
	in->hdr_chunk = 0;

	yaffs_free_obj(in);
	return YAFFS_OK;

}

/* yaffs_del_file deletes the whole file data
 * and the inode associated with the file.
 * It does not delete the links associated with the file.
 */
static int yaffs_unlink_file_if_needed(yaffs_obj_t *in)
{

	int retVal;
	int immediateDeletion = 0;
	yaffs_dev_t *dev = in->my_dev;

	if (!in->my_inode)
		immediateDeletion = 1;

	if (immediateDeletion) {
		retVal =
		    yaffs_change_obj_name(in, in->my_dev->del_dir,
					   _Y("deleted"), 0, 0);
		T(YAFFS_TRACE_TRACING,
		  (TSTR("yaffs: immediate deletion of file %d" TENDSTR),
		   in->obj_id));
		in->deleted = 1;
		in->my_dev->n_deleted_files++;
		if (dev->param.disable_soft_del || dev->param.is_yaffs2)
			yaffs_resize_file(in, 0);
		yaffs_soft_del_file(in);
	} else {
		retVal =
		    yaffs_change_obj_name(in, in->my_dev->unlinked_dir,
					   _Y("unlinked"), 0, 0);
	}


	return retVal;
}

int yaffs_del_file(yaffs_obj_t *in)
{
	int retVal = YAFFS_OK;
	int deleted; /* Need to cache value on stack if in is freed */
	yaffs_dev_t *dev = in->my_dev;

	if (dev->param.disable_soft_del || dev->param.is_yaffs2)
		yaffs_resize_file(in, 0);

	if (in->n_data_chunks > 0) {
		/* Use soft deletion if there is data in the file.
		 * That won't be the case if it has been resized to zero.
		 */
		if (!in->unlinked)
			retVal = yaffs_unlink_file_if_needed(in);

		deleted = in->deleted;

		if (retVal == YAFFS_OK && in->unlinked && !in->deleted) {
			in->deleted = 1;
			deleted = 1;
			in->my_dev->n_deleted_files++;
			yaffs_soft_del_file(in);
		}
		return deleted ? YAFFS_OK : YAFFS_FAIL;
	} else {
		/* The file has no data chunks so we toss it immediately */
		yaffs_free_tnode(in->my_dev, in->variant.file_variant.top);
		in->variant.file_variant.top = NULL;
		yaffs_generic_obj_del(in);

		return YAFFS_OK;
	}
}

static int yaffs_is_non_empty_dir(yaffs_obj_t *obj)
{
	return (obj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY) &&
		!(ylist_empty(&obj->variant.dir_variant.children));
}

static int yaffs_del_dir(yaffs_obj_t *obj)
{
	/* First check that the directory is empty. */
	if (yaffs_is_non_empty_dir(obj))
		return YAFFS_FAIL;

	return yaffs_generic_obj_del(obj);
}

static int yaffs_del_symlink(yaffs_obj_t *in)
{
	if(in->variant.symlink_variant.alias)
		YFREE(in->variant.symlink_variant.alias);
	in->variant.symlink_variant.alias=NULL;

	return yaffs_generic_obj_del(in);
}

static int yaffs_del_link(yaffs_obj_t *in)
{
	/* remove this hardlink from the list assocaited with the equivalent
	 * object
	 */
	ylist_del_init(&in->hard_links);
	return yaffs_generic_obj_del(in);
}

int yaffs_del_obj(yaffs_obj_t *obj)
{
int retVal = -1;
	switch (obj->variant_type) {
	case YAFFS_OBJECT_TYPE_FILE:
		retVal = yaffs_del_file(obj);
		break;
	case YAFFS_OBJECT_TYPE_DIRECTORY:
		if(!ylist_empty(&obj->variant.dir_variant.dirty)){
			T(YAFFS_TRACE_BACKGROUND, (TSTR("Remove object %d from dirty directories" TENDSTR),obj->obj_id));
			ylist_del_init(&obj->variant.dir_variant.dirty);
		}
		return yaffs_del_dir(obj);
		break;
	case YAFFS_OBJECT_TYPE_SYMLINK:
		retVal = yaffs_del_symlink(obj);
		break;
	case YAFFS_OBJECT_TYPE_HARDLINK:
		retVal = yaffs_del_link(obj);
		break;
	case YAFFS_OBJECT_TYPE_SPECIAL:
		retVal = yaffs_generic_obj_del(obj);
		break;
	case YAFFS_OBJECT_TYPE_UNKNOWN:
		retVal = 0;
		break;		/* should not happen. */
	}

	return retVal;
}

static int yaffs_unlink_worker(yaffs_obj_t *obj)
{

	int immediateDeletion = 0;

	if (!obj->my_inode)
		immediateDeletion = 1;

	if(obj)
		yaffs_update_parent(obj->parent);

	if (obj->variant_type == YAFFS_OBJECT_TYPE_HARDLINK) {
		return yaffs_del_link(obj);
	} else if (!ylist_empty(&obj->hard_links)) {
		/* Curve ball: We're unlinking an object that has a hardlink.
		 *
		 * This problem arises because we are not strictly following
		 * The Linux link/inode model.
		 *
		 * We can't really delete the object.
		 * Instead, we do the following:
		 * - Select a hardlink.
		 * - Unhook it from the hard links
		 * - Move it from its parent directory (so that the rename can work)
		 * - Rename the object to the hardlink's name.
		 * - Delete the hardlink
		 */

		yaffs_obj_t *hl;
		yaffs_obj_t *parent;
		int retVal;
		YCHAR name[YAFFS_MAX_NAME_LENGTH + 1];

		hl = ylist_entry(obj->hard_links.next, yaffs_obj_t, hard_links);

		yaffs_get_obj_name(hl, name, YAFFS_MAX_NAME_LENGTH + 1);
		parent = hl->parent;

		ylist_del_init(&hl->hard_links);

 		yaffs_add_obj_to_dir(obj->my_dev->unlinked_dir, hl);

		retVal = yaffs_change_obj_name(obj,parent, name, 0, 0);

		if (retVal == YAFFS_OK)
			retVal = yaffs_generic_obj_del(hl);

		return retVal;

	} else if (immediateDeletion) {
		switch (obj->variant_type) {
		case YAFFS_OBJECT_TYPE_FILE:
			return yaffs_del_file(obj);
			break;
		case YAFFS_OBJECT_TYPE_DIRECTORY:
			ylist_del_init(&obj->variant.dir_variant.dirty);
			return yaffs_del_dir(obj);
			break;
		case YAFFS_OBJECT_TYPE_SYMLINK:
			return yaffs_del_symlink(obj);
			break;
		case YAFFS_OBJECT_TYPE_SPECIAL:
			return yaffs_generic_obj_del(obj);
			break;
		case YAFFS_OBJECT_TYPE_HARDLINK:
		case YAFFS_OBJECT_TYPE_UNKNOWN:
		default:
			return YAFFS_FAIL;
		}
	} else if(yaffs_is_non_empty_dir(obj))
		return YAFFS_FAIL;
	else
		return yaffs_change_obj_name(obj, obj->my_dev->unlinked_dir,
					   _Y("unlinked"), 0, 0);
}


static int yaffs_unlink_obj(yaffs_obj_t *obj)
{

	if (obj && obj->unlink_allowed)
		return yaffs_unlink_worker(obj);

	return YAFFS_FAIL;

}
int yaffs_unlinker(yaffs_obj_t *dir, const YCHAR *name)
{
	yaffs_obj_t *obj;

	obj = yaffs_find_by_name(dir, name);
	return yaffs_unlink_obj(obj);
}

/*----------------------- Initialisation Scanning ---------------------- */

void yaffs_handle_shadowed_obj(yaffs_dev_t *dev, int obj_id,
				int backward_scanning)
{
	yaffs_obj_t *obj;

	if (!backward_scanning) {
		/* Handle YAFFS1 forward scanning case
		 * For YAFFS1 we always do the deletion
		 */

	} else {
		/* Handle YAFFS2 case (backward scanning)
		 * If the shadowed object exists then ignore.
		 */
		obj = yaffs_find_by_number(dev, obj_id);
		if(obj)
			return;
	}

	/* Let's create it (if it does not exist) assuming it is a file so that it can do shrinking etc.
	 * We put it in unlinked dir to be cleaned up after the scanning
	 */
	obj =
	    yaffs_find_or_create_by_number(dev, obj_id,
					     YAFFS_OBJECT_TYPE_FILE);
	if (!obj)
		return;
	obj->is_shadowed = 1;
	yaffs_add_obj_to_dir(dev->unlinked_dir, obj);
	obj->variant.file_variant.shrink_size = 0;
	obj->valid = 1;		/* So that we don't read any other info for this file */

}


void yaffs_link_fixup(yaffs_dev_t *dev, yaffs_obj_t *hard_list)
{
	yaffs_obj_t *hl;
	yaffs_obj_t *in;

	while (hard_list) {
		hl = hard_list;
		hard_list = (yaffs_obj_t *) (hard_list->hard_links.next);

		in = yaffs_find_by_number(dev,
					      hl->variant.hardlink_variant.
					      equiv_id);

		if (in) {
			/* Add the hardlink pointers */
			hl->variant.hardlink_variant.equiv_obj = in;
			ylist_add(&hl->hard_links, &in->hard_links);
		} else {
			/* Todo Need to report/handle this better.
			 * Got a problem... hardlink to a non-existant object
			 */
			hl->variant.hardlink_variant.equiv_obj = NULL;
			YINIT_LIST_HEAD(&hl->hard_links);

		}
	}
}


static void yaffs_strip_deleted_objs(yaffs_dev_t *dev)
{
	/*
	*  Sort out state of unlinked and deleted objects after scanning.
	*/
	struct ylist_head *i;
	struct ylist_head *n;
	yaffs_obj_t *l;

	if (dev->read_only)
		return;

	/* Soft delete all the unlinked files */
	ylist_for_each_safe(i, n,
		&dev->unlinked_dir->variant.dir_variant.children) {
		if (i) {
			l = ylist_entry(i, yaffs_obj_t, siblings);
			yaffs_del_obj(l);
		}
	}

	ylist_for_each_safe(i, n,
		&dev->del_dir->variant.dir_variant.children) {
		if (i) {
			l = ylist_entry(i, yaffs_obj_t, siblings);
			yaffs_del_obj(l);
		}
	}

}

/*
 * This code iterates through all the objects making sure that they are rooted.
 * Any unrooted objects are re-rooted in lost+found.
 * An object needs to be in one of:
 * - Directly under deleted, unlinked
 * - Directly or indirectly under root.
 *
 * Note:
 *  This code assumes that we don't ever change the current relationships between
 *  directories:
 *   root_dir->parent == unlinked_dir->parent == del_dir->parent == NULL
 *   lostNfound->parent == root_dir
 *
 * This fixes the problem where directories might have inadvertently been deleted
 * leaving the object "hanging" without being rooted in the directory tree.
 */
 
static int yaffs_has_null_parent(yaffs_dev_t *dev, yaffs_obj_t *obj)
{
	return (obj == dev->del_dir ||
		obj == dev->unlinked_dir||
		obj == dev->root_dir);
}

static void yaffs_fix_hanging_objs(yaffs_dev_t *dev)
{
	yaffs_obj_t *obj;
	yaffs_obj_t *parent;
	int i;
	struct ylist_head *lh;
	struct ylist_head *n;
	int depthLimit;
	int hanging;

	if (dev->read_only)
		return;

	/* Iterate through the objects in each hash entry,
	 * looking at each object.
	 * Make sure it is rooted.
	 */

	for (i = 0; i <  YAFFS_NOBJECT_BUCKETS; i++) {
		ylist_for_each_safe(lh, n, &dev->obj_bucket[i].list) {
			if (lh) {
				obj = ylist_entry(lh, yaffs_obj_t, hash_link);
				parent= obj->parent;
				
				if(yaffs_has_null_parent(dev,obj)){
					/* These directories are not hanging */
					hanging = 0;
				}
				else if(!parent || parent->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY)
					hanging = 1;
				else if(yaffs_has_null_parent(dev,parent))
					hanging = 0;
				else {
					/*
					 * Need to follow the parent chain to see if it is hanging.
					 */
					hanging = 0;
					depthLimit=100;

					while(parent != dev->root_dir &&
						parent->parent &&
						parent->parent->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY &&
						depthLimit > 0){
						parent = parent->parent;
						depthLimit--;
					}
					if(parent != dev->root_dir)
						hanging = 1;
				}
				if(hanging){
					T(YAFFS_TRACE_SCAN,
					  (TSTR("Hanging object %d moved to lost and found" TENDSTR),
					  	obj->obj_id));
					yaffs_add_obj_to_dir(dev->lost_n_found,obj);
				}
			}
		}
	}
}


/*
 * Delete directory contents for cleaning up lost and found.
 */
static void yaffs_del_dir_contents(yaffs_obj_t *dir)
{
	yaffs_obj_t *obj;
	struct ylist_head *lh;
	struct ylist_head *n;

	if(dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY)
		YBUG();
	
	ylist_for_each_safe(lh, n, &dir->variant.dir_variant.children) {
		if (lh) {
			obj = ylist_entry(lh, yaffs_obj_t, siblings);
			if(obj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY)
				yaffs_del_dir_contents(obj);

			T(YAFFS_TRACE_SCAN,
				(TSTR("Deleting lost_found object %d" TENDSTR),
				obj->obj_id));

			/* Need to use UnlinkObject since Delete would not handle
			 * hardlinked objects correctly.
			 */
			yaffs_unlink_obj(obj); 
		}
	}
			
}

static void yaffs_empty_l_n_f(yaffs_dev_t *dev)
{
	yaffs_del_dir_contents(dev->lost_n_found);
}

static void yaffs_check_obj_details_loaded(yaffs_obj_t *in)
{
	__u8 *chunkData;
	yaffs_obj_header *oh;
	yaffs_dev_t *dev;
	yaffs_ext_tags tags;
	int result;
	int alloc_failed = 0;

	if (!in)
		return;

	dev = in->my_dev;

#if 0
	T(YAFFS_TRACE_SCAN, (TSTR("details for object %d %s loaded" TENDSTR),
		in->obj_id,
		in->lazy_loaded ? "not yet" : "already"));
#endif

	if (in->lazy_loaded && in->hdr_chunk > 0) {
		in->lazy_loaded = 0;
		chunkData = yaffs_get_temp_buffer(dev, __LINE__);

		result = yaffs_rd_chunk_tags_nand(dev, in->hdr_chunk, chunkData, &tags);
		oh = (yaffs_obj_header *) chunkData;

		in->yst_mode = oh->yst_mode;
#ifdef CONFIG_YAFFS_WINCE
		in->win_atime[0] = oh->win_atime[0];
		in->win_ctime[0] = oh->win_ctime[0];
		in->win_mtime[0] = oh->win_mtime[0];
		in->win_atime[1] = oh->win_atime[1];
		in->win_ctime[1] = oh->win_ctime[1];
		in->win_mtime[1] = oh->win_mtime[1];
#else
		in->yst_uid = oh->yst_uid;
		in->yst_gid = oh->yst_gid;
		in->yst_atime = oh->yst_atime;
		in->yst_mtime = oh->yst_mtime;
		in->yst_ctime = oh->yst_ctime;
		in->yst_rdev = oh->yst_rdev;

#endif
		yaffs_set_obj_name_from_oh(in, oh);

		if (in->variant_type == YAFFS_OBJECT_TYPE_SYMLINK) {
			in->variant.symlink_variant.alias =
						    yaffs_clone_str(oh->alias);
			if (!in->variant.symlink_variant.alias)
				alloc_failed = 1; /* Not returned to caller */
		}

		yaffs_release_temp_buffer(dev, chunkData, __LINE__);
	}
}

/*------------------------------  Directory Functions ----------------------------- */

/*
 *yaffs_update_parent() handles fixing a directories mtime and ctime when a new
 * link (ie. name) is created or deleted in the directory.
 *
 * ie.
 *   create dir/a : update dir's mtime/ctime
 *   rm dir/a:   update dir's mtime/ctime
 *   modify dir/a: don't update dir's mtimme/ctime
 *
 * This can be handled immediately or defered. Defering helps reduce the number
 * of updates when many files in a directory are changed within a brief period.
 *
 * If the directory updating is defered then yaffs_update_dirty_dirs must be
 * called periodically.
 */
 
static void yaffs_update_parent(yaffs_obj_t *obj)
{
	yaffs_dev_t *dev;
	if(!obj)
		return;
#ifndef CONFIG_YAFFS_WINCE

	dev = obj->my_dev;
	obj->dirty = 1;
	obj->yst_mtime = obj->yst_ctime = Y_CURRENT_TIME;
	if(dev->param.defered_dir_update){
		struct ylist_head *link = &obj->variant.dir_variant.dirty; 
	
		if(ylist_empty(link)){
			ylist_add(link,&dev->dirty_dirs);
			T(YAFFS_TRACE_BACKGROUND, (TSTR("Added object %d to dirty directories" TENDSTR),obj->obj_id));
		}

	} else
		yaffs_update_oh(obj, NULL, 0, 0, 0, NULL);
#endif
}

void yaffs_update_dirty_dirs(yaffs_dev_t *dev)
{
	struct ylist_head *link;
	yaffs_obj_t *obj;
	yaffs_dir_s *dS;
	yaffs_obj_variant *oV;

	T(YAFFS_TRACE_BACKGROUND, (TSTR("Update dirty directories" TENDSTR)));

	while(!ylist_empty(&dev->dirty_dirs)){
		link = dev->dirty_dirs.next;
		ylist_del_init(link);
		
		dS=ylist_entry(link,yaffs_dir_s,dirty);
		oV = ylist_entry(dS,yaffs_obj_variant,dir_variant);
		obj = ylist_entry(oV,yaffs_obj_t,variant);

		T(YAFFS_TRACE_BACKGROUND, (TSTR("Update directory %d" TENDSTR), obj->obj_id));

		if(obj->dirty)
			yaffs_update_oh(obj, NULL, 0, 0, 0, NULL);
	}
}

static void yaffs_remove_obj_from_dir(yaffs_obj_t *obj)
{
	yaffs_dev_t *dev = obj->my_dev;
	yaffs_obj_t *parent;

	yaffs_verify_obj_in_dir(obj);
	parent = obj->parent;

	yaffs_verify_dir(parent);

	if (dev && dev->param.remove_obj_fn)
		dev->param.remove_obj_fn(obj);


	ylist_del_init(&obj->siblings);
	obj->parent = NULL;
	
	yaffs_verify_dir(parent);
}

void yaffs_add_obj_to_dir(yaffs_obj_t *directory,
					yaffs_obj_t *obj)
{
	if (!directory) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("tragedy: Trying to add an object to a null pointer directory"
		    TENDSTR)));
		YBUG();
		return;
	}
	if (directory->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("tragedy: Trying to add an object to a non-directory"
		    TENDSTR)));
		YBUG();
	}

	if (obj->siblings.prev == NULL) {
		/* Not initialised */
		YBUG();
	}


	yaffs_verify_dir(directory);

	yaffs_remove_obj_from_dir(obj);


	/* Now add it */
	ylist_add(&obj->siblings, &directory->variant.dir_variant.children);
	obj->parent = directory;

	if (directory == obj->my_dev->unlinked_dir
			|| directory == obj->my_dev->del_dir) {
		obj->unlinked = 1;
		obj->my_dev->n_unlinked_files++;
		obj->rename_allowed = 0;
	}

	yaffs_verify_dir(directory);
	yaffs_verify_obj_in_dir(obj);
}

yaffs_obj_t *yaffs_find_by_name(yaffs_obj_t *directory,
				     const YCHAR *name)
{
	int sum;

	struct ylist_head *i;
	YCHAR buffer[YAFFS_MAX_NAME_LENGTH + 1];

	yaffs_obj_t *l;

	if (!name)
		return NULL;

	if (!directory) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("tragedy: yaffs_find_by_name: null pointer directory"
		    TENDSTR)));
		YBUG();
		return NULL;
	}
	if (directory->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("tragedy: yaffs_find_by_name: non-directory" TENDSTR)));
		YBUG();
	}

	sum = yaffs_calc_name_sum(name);

	ylist_for_each(i, &directory->variant.dir_variant.children) {
		if (i) {
			l = ylist_entry(i, yaffs_obj_t, siblings);

			if (l->parent != directory)
				YBUG();

			yaffs_check_obj_details_loaded(l);

			/* Special case for lost-n-found */
			if (l->obj_id == YAFFS_OBJECTID_LOSTNFOUND) {
				if (yaffs_strcmp(name, YAFFS_LOSTNFOUND_NAME) == 0)
					return l;
			} else if (yaffs_sum_cmp(l->sum, sum) || l->hdr_chunk <= 0) {
				/* LostnFound chunk called Objxxx
				 * Do a real check
				 */
				yaffs_get_obj_name(l, buffer,
						    YAFFS_MAX_NAME_LENGTH + 1);
				if (yaffs_strncmp(name, buffer, YAFFS_MAX_NAME_LENGTH) == 0)
					return l;
			}
		}
	}

	return NULL;
}


#if 0
int yaffs_ApplyToDirectoryChildren(yaffs_obj_t *the_dir,
					int (*fn) (yaffs_obj_t *))
{
	struct ylist_head *i;
	yaffs_obj_t *l;

	if (!the_dir) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("tragedy: yaffs_find_by_name: null pointer directory"
		    TENDSTR)));
		YBUG();
		return YAFFS_FAIL;
	}
	if (the_dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("tragedy: yaffs_find_by_name: non-directory" TENDSTR)));
		YBUG();
		return YAFFS_FAIL;
	}

	ylist_for_each(i, &the_dir->variant.dir_variant.children) {
		if (i) {
			l = ylist_entry(i, yaffs_obj_t, siblings);
			if (l && !fn(l))
				return YAFFS_FAIL;
		}
	}

	return YAFFS_OK;

}
#endif

/* GetEquivalentObject dereferences any hard links to get to the
 * actual object.
 */

yaffs_obj_t *yaffs_get_equivalent_obj(yaffs_obj_t *obj)
{
	if (obj && obj->variant_type == YAFFS_OBJECT_TYPE_HARDLINK) {
		/* We want the object id of the equivalent object, not this one */
		obj = obj->variant.hardlink_variant.equiv_obj;
		yaffs_check_obj_details_loaded(obj);
	}
	return obj;
}

/*
 *  A note or two on object names.
 *  * If the object name is missing, we then make one up in the form objnnn
 *
 *  * ASCII names are stored in the object header's name field from byte zero
 *  * Unicode names are historically stored starting from byte zero.
 *
 * Then there are automatic Unicode names...
 * The purpose of these is to save names in a way that can be read as
 * ASCII or Unicode names as appropriate, thus allowing a Unicode and ASCII
 * system to share files.
 *
 * These automatic unicode are stored slightly differently...
 *  - If the name can fit in the ASCII character space then they are saved as 
 *    ascii names as per above.
 *  - If the name needs Unicode then the name is saved in Unicode
 *    starting at oh->name[1].

 */
static void yaffs_fix_null_name(yaffs_obj_t * obj,YCHAR * name, int buffer_size)
{
	/* Create an object name if we could not find one. */
	if(yaffs_strnlen(name,YAFFS_MAX_NAME_LENGTH) == 0){
		YCHAR locName[20];
		YCHAR numString[20];
		YCHAR *x = &numString[19];
		unsigned v = obj->obj_id;
		numString[19] = 0;
		while(v>0){
			x--;
			*x = '0' + (v % 10);
			v /= 10;
		}
		/* make up a name */
		yaffs_strcpy(locName, YAFFS_LOSTNFOUND_PREFIX);
		yaffs_strcat(locName,x);
		yaffs_strncpy(name, locName, buffer_size - 1);
	}
}

static void yaffs_load_name_from_oh(yaffs_dev_t *dev,YCHAR *name, const YCHAR *ohName, int bufferSize)
{
#ifdef CONFIG_YAFFS_AUTO_UNICODE
	if(dev->param.auto_unicode){
		if(*ohName){
			/* It is an ASCII name, so do an ASCII to unicode conversion */
			const char *asciiOhName = (const char *)ohName;
			int n = bufferSize - 1;
			while(n > 0 && *asciiOhName){
				*name = *asciiOhName;
				name++;
				asciiOhName++;
				n--;
			}
		} else 
			yaffs_strncpy(name,ohName+1, bufferSize -1);
	} else
#endif
		yaffs_strncpy(name, ohName, bufferSize - 1);
}


static void yaffs_load_oh_from_name(yaffs_dev_t *dev, YCHAR *ohName, const YCHAR *name)
{
#ifdef CONFIG_YAFFS_AUTO_UNICODE

	int isAscii;
	YCHAR *w;

	if(dev->param.auto_unicode){

		isAscii = 1;
		w = name;
	
		/* Figure out if the name will fit in ascii character set */
		while(isAscii && *w){
			if((*w) & 0xff00)
				isAscii = 0;
			w++;
		}

		if(isAscii){
			/* It is an ASCII name, so do a unicode to ascii conversion */
			char *asciiOhName = (char *)ohName;
			int n = YAFFS_MAX_NAME_LENGTH  - 1;
			while(n > 0 && *name){
				*asciiOhName= *name;
				name++;
				asciiOhName++;
				n--;
			}
		} else{
			/* It is a unicode name, so save starting at the second YCHAR */
			*ohName = 0;
			yaffs_strncpy(ohName+1,name, YAFFS_MAX_NAME_LENGTH -2);
		}
	}
	else 
#endif
		yaffs_strncpy(ohName,name, YAFFS_MAX_NAME_LENGTH - 1);

}

int yaffs_get_obj_name(yaffs_obj_t * obj, YCHAR * name, int buffer_size)
{
	memset(name, 0, buffer_size * sizeof(YCHAR));
	
	yaffs_check_obj_details_loaded(obj);

	if (obj->obj_id == YAFFS_OBJECTID_LOSTNFOUND) {
		yaffs_strncpy(name, YAFFS_LOSTNFOUND_NAME, buffer_size - 1);
	} 
#ifdef CONFIG_YAFFS_SHORT_NAMES_IN_RAM
	else if (obj->short_name[0]) {
		yaffs_strcpy(name, obj->short_name);
	}
#endif
	else if(obj->hdr_chunk > 0) {
		int result;
		__u8 *buffer = yaffs_get_temp_buffer(obj->my_dev, __LINE__);

		yaffs_obj_header *oh = (yaffs_obj_header *) buffer;

		memset(buffer, 0, obj->my_dev->data_bytes_per_chunk);

		if (obj->hdr_chunk > 0) {
			result = yaffs_rd_chunk_tags_nand(obj->my_dev,
							obj->hdr_chunk, buffer,
							NULL);
		}
		yaffs_load_name_from_oh(obj->my_dev,name,oh->name,buffer_size);

		yaffs_release_temp_buffer(obj->my_dev, buffer, __LINE__);
	}

	yaffs_fix_null_name(obj,name,buffer_size);

	return yaffs_strnlen(name,YAFFS_MAX_NAME_LENGTH);
}


int yaffs_get_obj_length(yaffs_obj_t *obj)
{
	/* Dereference any hard linking */
	obj = yaffs_get_equivalent_obj(obj);

	if (obj->variant_type == YAFFS_OBJECT_TYPE_FILE)
		return obj->variant.file_variant.file_size;
	if (obj->variant_type == YAFFS_OBJECT_TYPE_SYMLINK){
		if(!obj->variant.symlink_variant.alias)
			return 0;
		return yaffs_strnlen(obj->variant.symlink_variant.alias,YAFFS_MAX_ALIAS_LENGTH);
	} else {
		/* Only a directory should drop through to here */
		return obj->my_dev->data_bytes_per_chunk;
	}
}

int yaffs_get_obj_link_count(yaffs_obj_t *obj)
{
	int count = 0;
	struct ylist_head *i;

	if (!obj->unlinked)
		count++;		/* the object itself */

	ylist_for_each(i, &obj->hard_links)
		count++;		/* add the hard links; */

	return count;
}

int yaffs_get_obj_inode(yaffs_obj_t *obj)
{
	obj = yaffs_get_equivalent_obj(obj);

	return obj->obj_id;
}

unsigned yaffs_get_obj_type(yaffs_obj_t *obj)
{
	obj = yaffs_get_equivalent_obj(obj);

	switch (obj->variant_type) {
	case YAFFS_OBJECT_TYPE_FILE:
		return DT_REG;
		break;
	case YAFFS_OBJECT_TYPE_DIRECTORY:
		return DT_DIR;
		break;
	case YAFFS_OBJECT_TYPE_SYMLINK:
		return DT_LNK;
		break;
	case YAFFS_OBJECT_TYPE_HARDLINK:
		return DT_REG;
		break;
	case YAFFS_OBJECT_TYPE_SPECIAL:
		if (S_ISFIFO(obj->yst_mode))
			return DT_FIFO;
		if (S_ISCHR(obj->yst_mode))
			return DT_CHR;
		if (S_ISBLK(obj->yst_mode))
			return DT_BLK;
		if (S_ISSOCK(obj->yst_mode))
			return DT_SOCK;
	default:
		return DT_REG;
		break;
	}
}

YCHAR *yaffs_get_symlink_alias(yaffs_obj_t *obj)
{
	obj = yaffs_get_equivalent_obj(obj);
	if (obj->variant_type == YAFFS_OBJECT_TYPE_SYMLINK)
		return yaffs_clone_str(obj->variant.symlink_variant.alias);
	else
		return yaffs_clone_str(_Y(""));
}

#ifndef CONFIG_YAFFS_WINCE

int yaffs_set_attribs(yaffs_obj_t *obj, struct iattr *attr)
{
	unsigned int valid = attr->ia_valid;

	if (valid & ATTR_MODE)
		obj->yst_mode = attr->ia_mode;
	if (valid & ATTR_UID)
		obj->yst_uid = attr->ia_uid;
	if (valid & ATTR_GID)
		obj->yst_gid = attr->ia_gid;

	if (valid & ATTR_ATIME)
		obj->yst_atime = Y_TIME_CONVERT(attr->ia_atime);
	if (valid & ATTR_CTIME)
		obj->yst_ctime = Y_TIME_CONVERT(attr->ia_ctime);
	if (valid & ATTR_MTIME)
		obj->yst_mtime = Y_TIME_CONVERT(attr->ia_mtime);

	if (valid & ATTR_SIZE)
		yaffs_resize_file(obj, attr->ia_size);

	yaffs_update_oh(obj, NULL, 1, 0, 0, NULL);

	return YAFFS_OK;

}
int yaffs_get_attribs(yaffs_obj_t *obj, struct iattr *attr)
{
	unsigned int valid = 0;

	attr->ia_mode = obj->yst_mode;
	valid |= ATTR_MODE;
	attr->ia_uid = obj->yst_uid;
	valid |= ATTR_UID;
	attr->ia_gid = obj->yst_gid;
	valid |= ATTR_GID;

	Y_TIME_CONVERT(attr->ia_atime) = obj->yst_atime;
	valid |= ATTR_ATIME;
	Y_TIME_CONVERT(attr->ia_ctime) = obj->yst_ctime;
	valid |= ATTR_CTIME;
	Y_TIME_CONVERT(attr->ia_mtime) = obj->yst_mtime;
	valid |= ATTR_MTIME;

	attr->ia_size = yaffs_get_file_size(obj);
	valid |= ATTR_SIZE;

	attr->ia_valid = valid;

	return YAFFS_OK;
}

#endif


static int yaffs_do_xattrib_mod(yaffs_obj_t *obj, int set, const YCHAR *name, const void *value, int size, int flags)
{
	yaffs_xattr_mod xmod;

	int result;

	xmod.set = set;
	xmod.name = name;
	xmod.data = value;
	xmod.size =  size;
	xmod.flags = flags;
	xmod.result = -ENOSPC;

	result = yaffs_update_oh(obj, NULL, 0, 0, 0, &xmod);

	if(result > 0)
		return xmod.result;
	else
		return -ENOSPC;
}

static int yaffs_apply_xattrib_mod(yaffs_obj_t *obj, char *buffer, yaffs_xattr_mod *xmod)
{
	int retval = 0;
	int x_offs = sizeof(yaffs_obj_header);
	yaffs_dev_t *dev = obj->my_dev;
	int x_size = dev->data_bytes_per_chunk - sizeof(yaffs_obj_header);

	char * x_buffer = buffer + x_offs;

	if(xmod->set)
		retval = nval_set(x_buffer, x_size, xmod->name, xmod->data, xmod->size, xmod->flags);
	else
		retval = nval_del(x_buffer, x_size, xmod->name);

	obj->has_xattr = nval_hasvalues(x_buffer, x_size);
	obj->xattr_known = 1;

	xmod->result = retval;

	return retval;
}

static int yaffs_do_xattrib_fetch(yaffs_obj_t *obj, const YCHAR *name, void *value, int size)
{
	char *buffer = NULL;
	int result;
	yaffs_ext_tags tags;
	yaffs_dev_t *dev = obj->my_dev;
	int x_offs = sizeof(yaffs_obj_header);
	int x_size = dev->data_bytes_per_chunk - sizeof(yaffs_obj_header);

	char * x_buffer;

	int retval = 0;

	if(obj->hdr_chunk < 1)
		return -ENODATA;

	/* If we know that the object has no xattribs then don't do all the
	 * reading and parsing.
	 */
	if(obj->xattr_known && !obj->has_xattr){
		if(name)
			return -ENODATA;
		else
			return 0;
	}

	buffer = (char *) yaffs_get_temp_buffer(dev, __LINE__);
	if(!buffer)
		return -ENOMEM;

	result = yaffs_rd_chunk_tags_nand(dev,obj->hdr_chunk, (__u8 *)buffer, &tags);

	if(result != YAFFS_OK)
		retval = -ENOENT;
	else{
		x_buffer =  buffer + x_offs;

		if (!obj->xattr_known){
			obj->has_xattr = nval_hasvalues(x_buffer, x_size);
			obj->xattr_known = 1;
		}

		if(name)
			retval = nval_get(x_buffer, x_size, name, value, size);
		else
			retval = nval_list(x_buffer, x_size, value,size);
	}
	yaffs_release_temp_buffer(dev,(__u8 *)buffer,__LINE__);
	return retval;
}

int yaffs_set_xattrib(yaffs_obj_t *obj, const YCHAR *name, const void * value, int size, int flags)
{
	return yaffs_do_xattrib_mod(obj, 1, name, value, size, flags);
}

int yaffs_remove_xattrib(yaffs_obj_t *obj, const YCHAR *name)
{
	return yaffs_do_xattrib_mod(obj, 0, name, NULL, 0, 0);
}

int yaffs_get_xattrib(yaffs_obj_t *obj, const YCHAR *name, void *value, int size)
{
	return yaffs_do_xattrib_fetch(obj, name, value, size);
}

int yaffs_list_xattrib(yaffs_obj_t *obj, char *buffer, int size)
{
	return yaffs_do_xattrib_fetch(obj, NULL, buffer,size);
}



#if 0
int yaffs_dump_obj(yaffs_obj_t *obj)
{
	YCHAR name[257];

	yaffs_get_obj_name(obj, name, YAFFS_MAX_NAME_LENGTH + 1);

	T(YAFFS_TRACE_ALWAYS,
	  (TSTR
	   ("Object %d, inode %d \"%s\"\n dirty %d valid %d serial %d sum %d"
	    " chunk %d type %d size %d\n"
	    TENDSTR), obj->obj_id, yaffs_get_obj_inode(obj), name,
	   obj->dirty, obj->valid, obj->serial, obj->sum, obj->hdr_chunk,
	   yaffs_get_obj_type(obj), yaffs_get_obj_length(obj)));

	return YAFFS_OK;
}
#endif

/*---------------------------- Initialisation code -------------------------------------- */

static int yaffs_cehck_dev_fns(const yaffs_dev_t *dev)
{

	/* Common functions, gotta have */
	if (!dev->param.erase_fn || !dev->param.initialise_flash_fn)
		return 0;

#ifdef CONFIG_YAFFS_YAFFS2

	/* Can use the "with tags" style interface for yaffs1 or yaffs2 */
	if (dev->param.write_chunk_tags_fn &&
	    dev->param.read_chunk_tags_fn &&
	    !dev->param.write_chunk_fn &&
	    !dev->param.read_chunk_fn &&
	    dev->param.bad_block_fn &&
	    dev->param.query_block_fn)
		return 1;
#endif

	/* Can use the "spare" style interface for yaffs1 */
	if (!dev->param.is_yaffs2 &&
	    !dev->param.write_chunk_tags_fn &&
	    !dev->param.read_chunk_tags_fn &&
	    dev->param.write_chunk_fn &&
	    dev->param.read_chunk_fn &&
	    !dev->param.bad_block_fn &&
	    !dev->param.query_block_fn)
		return 1;

	return 0;	/* bad */
}


static int yaffs_create_initial_dir(yaffs_dev_t *dev)
{
	/* Initialise the unlinked, deleted, root and lost and found directories */

	dev->lost_n_found = dev->root_dir =  NULL;
	dev->unlinked_dir = dev->del_dir = NULL;

	dev->unlinked_dir =
	    yaffs_create_fake_dir(dev, YAFFS_OBJECTID_UNLINKED, S_IFDIR);

	dev->del_dir =
	    yaffs_create_fake_dir(dev, YAFFS_OBJECTID_DELETED, S_IFDIR);

	dev->root_dir =
	    yaffs_create_fake_dir(dev, YAFFS_OBJECTID_ROOT,
				      YAFFS_ROOT_MODE | S_IFDIR);
	dev->lost_n_found =
	    yaffs_create_fake_dir(dev, YAFFS_OBJECTID_LOSTNFOUND,
				      YAFFS_LOSTNFOUND_MODE | S_IFDIR);

	if (dev->lost_n_found && dev->root_dir && dev->unlinked_dir && dev->del_dir) {
		yaffs_add_obj_to_dir(dev->root_dir, dev->lost_n_found);
		return YAFFS_OK;
	}

	return YAFFS_FAIL;
}

int yaffs_guts_initialise(yaffs_dev_t *dev)
{
	int init_failed = 0;
	unsigned x;
	int bits;

	T(YAFFS_TRACE_TRACING, (TSTR("yaffs: yaffs_guts_initialise()" TENDSTR)));

	/* Check stuff that must be set */

	if (!dev) {
		T(YAFFS_TRACE_ALWAYS, (TSTR("yaffs: Need a device" TENDSTR)));
		return YAFFS_FAIL;
	}

	dev->internal_start_block = dev->param.start_block;
	dev->internal_end_block = dev->param.end_block;
	dev->block_offset = 0;
	dev->chunk_offset = 0;
	dev->n_free_chunks = 0;

	dev->gc_block = 0;

	if (dev->param.start_block == 0) {
		dev->internal_start_block = dev->param.start_block + 1;
		dev->internal_end_block = dev->param.end_block + 1;
		dev->block_offset = 1;
		dev->chunk_offset = dev->param.chunks_per_block;
	}

	/* Check geometry parameters. */

	if ((!dev->param.inband_tags && dev->param.is_yaffs2 && dev->param.total_bytes_per_chunk < 1024) ||
	    (!dev->param.is_yaffs2 && dev->param.total_bytes_per_chunk < 512) ||
	    (dev->param.inband_tags && !dev->param.is_yaffs2) ||
	     dev->param.chunks_per_block < 2 ||
	     dev->param.n_reserved_blocks < 2 ||
	     dev->internal_start_block <= 0 ||
	     dev->internal_end_block <= 0 ||
	     dev->internal_end_block <= (dev->internal_start_block + dev->param.n_reserved_blocks + 2)) {	/* otherwise it is too small */
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("yaffs: NAND geometry problems: chunk size %d, type is yaffs%s, inband_tags %d "
		    TENDSTR), dev->param.total_bytes_per_chunk, dev->param.is_yaffs2 ? "2" : "", dev->param.inband_tags));
		return YAFFS_FAIL;
	}

	if (yaffs_init_nand(dev) != YAFFS_OK) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR("yaffs: InitialiseNAND failed" TENDSTR)));
		return YAFFS_FAIL;
	}

	/* Sort out space for inband tags, if required */
	if (dev->param.inband_tags)
		dev->data_bytes_per_chunk = dev->param.total_bytes_per_chunk - sizeof(yaffs_PackedTags2TagsPart);
	else
		dev->data_bytes_per_chunk = dev->param.total_bytes_per_chunk;

	/* Got the right mix of functions? */
	if (!yaffs_cehck_dev_fns(dev)) {
		/* Function missing */
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("yaffs: device function(s) missing or wrong\n" TENDSTR)));

		return YAFFS_FAIL;
	}

	/* This is really a compilation check. */
	if (!yaffs_check_structures()) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR("yaffs_check_structures failed\n" TENDSTR)));
		return YAFFS_FAIL;
	}

	if (dev->is_mounted) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR("yaffs: device already mounted\n" TENDSTR)));
		return YAFFS_FAIL;
	}

	/* Finished with most checks. One or two more checks happen later on too. */

	dev->is_mounted = 1;

	/* OK now calculate a few things for the device */

	/*
	 *  Calculate all the chunk size manipulation numbers:
	 */
	x = dev->data_bytes_per_chunk;
	/* We always use dev->chunk_shift and dev->chunk_div */
	dev->chunk_shift = Shifts(x);
	x >>= dev->chunk_shift;
	dev->chunk_div = x;
	/* We only use chunk mask if chunk_div is 1 */
	dev->chunk_mask = (1<<dev->chunk_shift) - 1;

	/*
	 * Calculate chunk_grp_bits.
	 * We need to find the next power of 2 > than internal_end_block
	 */

	x = dev->param.chunks_per_block * (dev->internal_end_block + 1);

	bits = ShiftsGE(x);

	/* Set up tnode width if wide tnodes are enabled. */
	if (!dev->param.wide_tnodes_disabled) {
		/* bits must be even so that we end up with 32-bit words */
		if (bits & 1)
			bits++;
		if (bits < 16)
			dev->tnode_width = 16;
		else
			dev->tnode_width = bits;
	} else
		dev->tnode_width = 16;

	dev->tnode_mask = (1<<dev->tnode_width)-1;

	/* Level0 Tnodes are 16 bits or wider (if wide tnodes are enabled),
	 * so if the bitwidth of the
	 * chunk range we're using is greater than 16 we need
	 * to figure out chunk shift and chunk_grp_size
	 */

	if (bits <= dev->tnode_width)
		dev->chunk_grp_bits = 0;
	else
		dev->chunk_grp_bits = bits - dev->tnode_width;

	dev->tnode_size = (dev->tnode_width * YAFFS_NTNODES_LEVEL0)/8;
	if(dev->tnode_size < sizeof(yaffs_tnode_t))
		dev->tnode_size = sizeof(yaffs_tnode_t);

	dev->chunk_grp_size = 1 << dev->chunk_grp_bits;

	if (dev->param.chunks_per_block < dev->chunk_grp_size) {
		/* We have a problem because the soft delete won't work if
		 * the chunk group size > chunks per block.
		 * This can be remedied by using larger "virtual blocks".
		 */
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR("yaffs: chunk group too large\n" TENDSTR)));

		return YAFFS_FAIL;
	}

	/* OK, we've finished verifying the device, lets continue with initialisation */

	/* More device initialisation */
	dev->all_gcs = 0;
	dev->passive_gc_count = 0;
	dev->oldest_dirty_gc_count = 0;
	dev->bg_gcs = 0;
	dev->gc_block_finder = 0;
	dev->buffered_block = -1;
	dev->doing_buffered_block_rewrite = 0;
	dev->n_deleted_files = 0;
	dev->n_bg_deletions = 0;
	dev->n_unlinked_files = 0;
	dev->n_ecc_fixed = 0;
	dev->n_ecc_unfixed = 0;
	dev->n_tags_ecc_fixed = 0;
	dev->n_tags_ecc_unfixed = 0;
	dev->n_erase_failures = 0;
	dev->n_erased_blocks = 0;
	dev->gc_disable= 0;
	dev->has_pending_prioritised_gc = 1; /* Assume the worst for now, will get fixed on first GC */
	YINIT_LIST_HEAD(&dev->dirty_dirs);
	dev->oldest_dirty_seq = 0;
	dev->oldest_dirty_block = 0;

	/* Initialise temporary buffers and caches. */
	if (!yaffs_init_tmp_buffers(dev))
		init_failed = 1;

	dev->cache = NULL;
	dev->gc_cleanup_list = NULL;


	if (!init_failed &&
	    dev->param.n_caches > 0) {
		int i;
		void *buf;
		int cacheBytes = dev->param.n_caches * sizeof(yaffs_cache_t);

		if (dev->param.n_caches > YAFFS_MAX_SHORT_OP_CACHES)
			dev->param.n_caches = YAFFS_MAX_SHORT_OP_CACHES;

		dev->cache =  YMALLOC(cacheBytes);

		buf = (__u8 *) dev->cache;

		if (dev->cache)
			memset(dev->cache, 0, cacheBytes);

		for (i = 0; i < dev->param.n_caches && buf; i++) {
			dev->cache[i].object = NULL;
			dev->cache[i].last_use = 0;
			dev->cache[i].dirty = 0;
			dev->cache[i].data = buf = YMALLOC_DMA(dev->param.total_bytes_per_chunk);
		}
		if (!buf)
			init_failed = 1;

		dev->cache_last_use = 0;
	}

	dev->cache_hits = 0;

	if (!init_failed) {
		dev->gc_cleanup_list = YMALLOC(dev->param.chunks_per_block * sizeof(__u32));
		if (!dev->gc_cleanup_list)
			init_failed = 1;
	}

	if (dev->param.is_yaffs2)
		dev->param.use_header_file_size = 1;

	if (!init_failed && !yaffs_init_blocks(dev))
		init_failed = 1;

	yaffs_init_tnodes_and_objs(dev);

	if (!init_failed && !yaffs_create_initial_dir(dev))
		init_failed = 1;


	if (!init_failed) {
		/* Now scan the flash. */
		if (dev->param.is_yaffs2) {
			if (yaffs2_checkpt_restore(dev)) {
				yaffs_check_obj_details_loaded(dev->root_dir);
				T(YAFFS_TRACE_ALWAYS,
				  (TSTR("yaffs: restored from checkpoint" TENDSTR)));
			} else {

				/* Clean up the mess caused by an aborted checkpoint load
				 * and scan backwards.
				 */
				yaffs_deinit_blocks(dev);

				yaffs_deinit_tnodes_and_objs(dev);

				dev->n_erased_blocks = 0;
				dev->n_free_chunks = 0;
				dev->alloc_block = -1;
				dev->alloc_page = -1;
				dev->n_deleted_files = 0;
				dev->n_unlinked_files = 0;
				dev->n_bg_deletions = 0;

				if (!init_failed && !yaffs_init_blocks(dev))
					init_failed = 1;

				yaffs_init_tnodes_and_objs(dev);

				if (!init_failed && !yaffs_create_initial_dir(dev))
					init_failed = 1;

				if (!init_failed && !yaffs2_scan_backwards(dev))
					init_failed = 1;
			}
		} else if (!yaffs1_scan(dev))
				init_failed = 1;

		yaffs_strip_deleted_objs(dev);
		yaffs_fix_hanging_objs(dev);
		if(dev->param.empty_lost_n_found)
			yaffs_empty_l_n_f(dev);
	}

	if (init_failed) {
		/* Clean up the mess */
		T(YAFFS_TRACE_TRACING,
		  (TSTR("yaffs: yaffs_guts_initialise() aborted.\n" TENDSTR)));

		yaffs_deinitialise(dev);
		return YAFFS_FAIL;
	}

	/* Zero out stats */
	dev->n_page_reads = 0;
	dev->n_page_writes = 0;
	dev->n_erasures = 0;
	dev->n_gc_copies = 0;
	dev->n_retired_writes = 0;

	dev->n_retired_blocks = 0;

	yaffs_verify_free_chunks(dev);
	yaffs_verify_blocks(dev);

	/* Clean up any aborted checkpoint data */
	if(!dev->is_checkpointed && dev->blocks_in_checkpt > 0)
		yaffs2_checkpt_invalidate(dev);

	T(YAFFS_TRACE_TRACING,
	  (TSTR("yaffs: yaffs_guts_initialise() done.\n" TENDSTR)));
	return YAFFS_OK;

}

void yaffs_deinitialise(yaffs_dev_t *dev)
{
	if (dev->is_mounted) {
		int i;

		yaffs_deinit_blocks(dev);
		yaffs_deinit_tnodes_and_objs(dev);
		if (dev->param.n_caches > 0 &&
		    dev->cache) {

			for (i = 0; i < dev->param.n_caches; i++) {
				if (dev->cache[i].data)
					YFREE(dev->cache[i].data);
				dev->cache[i].data = NULL;
			}

			YFREE(dev->cache);
			dev->cache = NULL;
		}

		YFREE(dev->gc_cleanup_list);

		for (i = 0; i < YAFFS_N_TEMP_BUFFERS; i++)
			YFREE(dev->temp_buffer[i].buffer);

		dev->is_mounted = 0;

		if (dev->param.deinitialise_flash_fn)
			dev->param.deinitialise_flash_fn(dev);
	}
}

int yaffs_count_free_chunks(yaffs_dev_t *dev)
{
	int nFree=0;
	int b;

	yaffs_block_info_t *blk;

	blk = dev->block_info;
	for (b = dev->internal_start_block; b <= dev->internal_end_block; b++) {
		switch (blk->block_state) {
		case YAFFS_BLOCK_STATE_EMPTY:
		case YAFFS_BLOCK_STATE_ALLOCATING:
		case YAFFS_BLOCK_STATE_COLLECTING:
		case YAFFS_BLOCK_STATE_FULL:
			nFree +=
			    (dev->param.chunks_per_block - blk->pages_in_use +
			     blk->soft_del_pages);
			break;
		default:
			break;
		}
		blk++;
	}

	return nFree;
}

int yaffs_get_n_free_chunks(yaffs_dev_t *dev)
{
	/* This is what we report to the outside world */

	int nFree;
	int nDirtyCacheChunks;
	int blocksForCheckpoint;
	int i;

#if 1
	nFree = dev->n_free_chunks;
#else
	nFree = yaffs_count_free_chunks(dev);
#endif

	nFree += dev->n_deleted_files;

	/* Now count the number of dirty chunks in the cache and subtract those */

	for (nDirtyCacheChunks = 0, i = 0; i < dev->param.n_caches; i++) {
		if (dev->cache[i].dirty)
			nDirtyCacheChunks++;
	}

	nFree -= nDirtyCacheChunks;

	nFree -= ((dev->param.n_reserved_blocks + 1) * dev->param.chunks_per_block);

	/* Now we figure out how much to reserve for the checkpoint and report that... */
	blocksForCheckpoint = yaffs_calc_checkpt_blocks_required(dev);

	nFree -= (blocksForCheckpoint * dev->param.chunks_per_block);

	if (nFree < 0)
		nFree = 0;

	return nFree;

}


/*---------------------------------------- YAFFS test code ----------------------*/

#define yaffs_check_struct(structure, syze, name) \
	do { \
		if (sizeof(structure) != syze) { \
			T(YAFFS_TRACE_ALWAYS, (TSTR("%s should be %d but is %d\n" TENDSTR),\
				name, syze, (int) sizeof(structure))); \
			return YAFFS_FAIL; \
		} \
	} while (0)

static int yaffs_check_structures(void)
{
/*      yaffs_check_struct(yaffs_tags_t,8,"yaffs_tags_t"); */
/*      yaffs_check_struct(yaffs_tags_union_t,8,"yaffs_tags_union_t"); */
/*      yaffs_check_struct(yaffs_spare,16,"yaffs_spare"); */
/*	yaffs_check_struct(yaffs_tnode_t, 2 * YAFFS_NTNODES_LEVEL0, "yaffs_tnode_t"); */

#ifndef CONFIG_YAFFS_WINCE
	yaffs_check_struct(yaffs_obj_header, 512, "yaffs_obj_header");
#endif
	return YAFFS_OK;
}
