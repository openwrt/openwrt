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

#ifndef __YAFFS_GUTS_H__
#define __YAFFS_GUTS_H__

#include "yportenv.h"
#include "devextras.h"
#include "yaffs_list.h"

#define YAFFS_OK	1
#define YAFFS_FAIL  0

/* Give us a  Y=0x59,
 * Give us an A=0x41,
 * Give us an FF=0xFF
 * Give us an S=0x53
 * And what have we got...
 */
#define YAFFS_MAGIC			0x5941FF53

#define YAFFS_NTNODES_LEVEL0	  	16
#define YAFFS_TNODES_LEVEL0_BITS	4
#define YAFFS_TNODES_LEVEL0_MASK	0xf

#define YAFFS_NTNODES_INTERNAL 		(YAFFS_NTNODES_LEVEL0 / 2)
#define YAFFS_TNODES_INTERNAL_BITS 	(YAFFS_TNODES_LEVEL0_BITS - 1)
#define YAFFS_TNODES_INTERNAL_MASK	0x7
#define YAFFS_TNODES_MAX_LEVEL		6

#ifndef CONFIG_YAFFS_NO_YAFFS1
#define YAFFS_BYTES_PER_SPARE		16
#define YAFFS_BYTES_PER_CHUNK		512
#define YAFFS_CHUNK_SIZE_SHIFT		9
#define YAFFS_CHUNKS_PER_BLOCK		32
#define YAFFS_BYTES_PER_BLOCK		(YAFFS_CHUNKS_PER_BLOCK*YAFFS_BYTES_PER_CHUNK)
#endif

#define YAFFS_MIN_YAFFS2_CHUNK_SIZE 	1024
#define YAFFS_MIN_YAFFS2_SPARE_SIZE	32

#define YAFFS_MAX_CHUNK_ID		0x000FFFFF


#define YAFFS_ALLOCATION_NOBJECTS	100
#define YAFFS_ALLOCATION_NTNODES	100
#define YAFFS_ALLOCATION_NLINKS		100

#define YAFFS_NOBJECT_BUCKETS		256


#define YAFFS_OBJECT_SPACE		0x40000
#define YAFFS_MAX_OBJECT_ID		(YAFFS_OBJECT_SPACE -1)

#define YAFFS_CHECKPOINT_VERSION 	4

#ifdef CONFIG_YAFFS_UNICODE
#define YAFFS_MAX_NAME_LENGTH		127
#define YAFFS_MAX_ALIAS_LENGTH		79
#else
#define YAFFS_MAX_NAME_LENGTH		255
#define YAFFS_MAX_ALIAS_LENGTH		159
#endif

#define YAFFS_SHORT_NAME_LENGTH		15

/* Some special object ids for pseudo objects */
#define YAFFS_OBJECTID_ROOT		1
#define YAFFS_OBJECTID_LOSTNFOUND	2
#define YAFFS_OBJECTID_UNLINKED		3
#define YAFFS_OBJECTID_DELETED		4

/* Pseudo object ids for checkpointing */
#define YAFFS_OBJECTID_SB_HEADER	0x10
#define YAFFS_OBJECTID_CHECKPOINT_DATA	0x20
#define YAFFS_SEQUENCE_CHECKPOINT_DATA  0x21


#define YAFFS_MAX_SHORT_OP_CACHES	20

#define YAFFS_N_TEMP_BUFFERS		6

/* We limit the number attempts at sucessfully saving a chunk of data.
 * Small-page devices have 32 pages per block; large-page devices have 64.
 * Default to something in the order of 5 to 10 blocks worth of chunks.
 */
#define YAFFS_WR_ATTEMPTS		(5*64)

/* Sequence numbers are used in YAFFS2 to determine block allocation order.
 * The range is limited slightly to help distinguish bad numbers from good.
 * This also allows us to perhaps in the future use special numbers for
 * special purposes.
 * EFFFFF00 allows the allocation of 8 blocks per second (~1Mbytes) for 15 years,
 * and is a larger number than the lifetime of a 2GB device.
 */
#define YAFFS_LOWEST_SEQUENCE_NUMBER	0x00001000
#define YAFFS_HIGHEST_SEQUENCE_NUMBER	0xEFFFFF00

/* Special sequence number for bad block that failed to be marked bad */
#define YAFFS_SEQUENCE_BAD_BLOCK	0xFFFF0000

/* ChunkCache is used for short read/write operations.*/
typedef struct {
	struct yaffs_obj_s *object;
	int chunk_id;
	int last_use;
	int dirty;
	int n_bytes;		/* Only valid if the cache is dirty */
	int locked;		/* Can't push out or flush while locked. */
	__u8 *data;
} yaffs_cache_t;



/* Tags structures in RAM
 * NB This uses bitfield. Bitfields should not straddle a u32 boundary otherwise
 * the structure size will get blown out.
 */

#ifndef CONFIG_YAFFS_NO_YAFFS1
typedef struct {
	unsigned chunk_id:20;
	unsigned serial_number:2;
	unsigned n_bytes_lsb:10;
	unsigned obj_id:18;
	unsigned ecc:12;
	unsigned n_bytes_msb:2;
} yaffs_tags_t;

typedef union {
	yaffs_tags_t as_tags;
	__u8 as_bytes[8];
} yaffs_tags_union_t;

#endif

/* Stuff used for extended tags in YAFFS2 */

typedef enum {
	YAFFS_ECC_RESULT_UNKNOWN,
	YAFFS_ECC_RESULT_NO_ERROR,
	YAFFS_ECC_RESULT_FIXED,
	YAFFS_ECC_RESULT_UNFIXED
} yaffs_ecc_result;

typedef enum {
	YAFFS_OBJECT_TYPE_UNKNOWN,
	YAFFS_OBJECT_TYPE_FILE,
	YAFFS_OBJECT_TYPE_SYMLINK,
	YAFFS_OBJECT_TYPE_DIRECTORY,
	YAFFS_OBJECT_TYPE_HARDLINK,
	YAFFS_OBJECT_TYPE_SPECIAL
} yaffs_obj_type;

#define YAFFS_OBJECT_TYPE_MAX YAFFS_OBJECT_TYPE_SPECIAL

typedef struct {

	unsigned validity1;
	unsigned chunk_used;	/*  Status of the chunk: used or unused */
	unsigned obj_id;	/* If 0 then this is not part of an object (unused) */
	unsigned chunk_id;	/* If 0 then this is a header, else a data chunk */
	unsigned n_bytes;	/* Only valid for data chunks */

	/* The following stuff only has meaning when we read */
	yaffs_ecc_result ecc_result;
	unsigned block_bad;

	/* YAFFS 1 stuff */
	unsigned is_deleted;	/* The chunk is marked deleted */
	unsigned serial_number;	/* Yaffs1 2-bit serial number */

	/* YAFFS2 stuff */
	unsigned seq_number;	/* The sequence number of this block */

	/* Extra info if this is an object header (YAFFS2 only) */

	unsigned extra_available;	/* There is extra info available if this is not zero */
	unsigned extra_parent_id;	/* The parent object */
	unsigned extra_is_shrink;	/* Is it a shrink header? */
	unsigned extra_shadows;		/* Does this shadow another object? */

	yaffs_obj_type extra_obj_type;	/* What object type? */

	unsigned extra_length;		/* Length if it is a file */
	unsigned extra_equiv_id;	/* Equivalent object Id if it is a hard link */

	unsigned validty1;

} yaffs_ext_tags;

/* Spare structure for YAFFS1 */
typedef struct {
	__u8 tb0;
	__u8 tb1;
	__u8 tb2;
	__u8 tb3;
	__u8 page_status;	/* set to 0 to delete the chunk */
	__u8 block_status;
	__u8 tb4;
	__u8 tb5;
	__u8 ecc1[3];
	__u8 tb6;
	__u8 tb7;
	__u8 ecc2[3];
} yaffs_spare;

/*Special structure for passing through to mtd */
struct yaffs_nand_spare {
	yaffs_spare spare;
	int eccres1;
	int eccres2;
};

/* Block data in RAM */

typedef enum {
	YAFFS_BLOCK_STATE_UNKNOWN = 0,

	YAFFS_BLOCK_STATE_SCANNING,
        /* Being scanned */

	YAFFS_BLOCK_STATE_NEEDS_SCANNING,
	/* The block might have something on it (ie it is allocating or full, perhaps empty)
	 * but it needs to be scanned to determine its true state.
	 * This state is only valid during yaffs_Scan.
	 * NB We tolerate empty because the pre-scanner might be incapable of deciding
	 * However, if this state is returned on a YAFFS2 device, then we expect a sequence number
	 */

	YAFFS_BLOCK_STATE_EMPTY,
	/* This block is empty */

	YAFFS_BLOCK_STATE_ALLOCATING,
	/* This block is partially allocated.
	 * At least one page holds valid data.
	 * This is the one currently being used for page
	 * allocation. Should never be more than one of these.
         * If a block is only partially allocated at mount it is treated as full.
	 */

	YAFFS_BLOCK_STATE_FULL,
	/* All the pages in this block have been allocated.
         * If a block was only partially allocated when mounted we treat
         * it as fully allocated.
	 */

	YAFFS_BLOCK_STATE_DIRTY,
	/* The block was full and now all chunks have been deleted.
	 * Erase me, reuse me.
	 */

	YAFFS_BLOCK_STATE_CHECKPOINT,
	/* This block is assigned to holding checkpoint data. */

	YAFFS_BLOCK_STATE_COLLECTING,
	/* This block is being garbage collected */

	YAFFS_BLOCK_STATE_DEAD
	/* This block has failed and is not in use */
} yaffs_block_state_t;

#define	YAFFS_NUMBER_OF_BLOCK_STATES (YAFFS_BLOCK_STATE_DEAD + 1)


typedef struct {

	int soft_del_pages:10;	/* number of soft deleted pages */
	int pages_in_use:10;	/* number of pages in use */
	unsigned block_state:4;	/* One of the above block states. NB use unsigned because enum is sometimes an int */
	__u32 needs_retiring:1;	/* Data has failed on this block, need to get valid data off */
				/* and retire the block. */
	__u32 skip_erased_check:1; /* If this is set we can skip the erased check on this block */
	__u32 gc_prioritise:1; 	/* An ECC check or blank check has failed on this block.
				   It should be prioritised for GC */
	__u32 chunk_error_strikes:3; /* How many times we've had ecc etc failures on this block and tried to reuse it */

#ifdef CONFIG_YAFFS_YAFFS2
	__u32 has_shrink_hdr:1; /* This block has at least one shrink object header */
	__u32 seq_number;	 /* block sequence number for yaffs2 */
#endif

} yaffs_block_info_t;

/* -------------------------- Object structure -------------------------------*/
/* This is the object structure as stored on NAND */

typedef struct {
	yaffs_obj_type type;

	/* Apply to everything  */
	int parent_obj_id;
	__u16 sum_no_longer_used;        /* checksum of name. No longer used */
	YCHAR name[YAFFS_MAX_NAME_LENGTH + 1];

	/* The following apply to directories, files, symlinks - not hard links */
	__u32 yst_mode;         /* protection */

#ifdef CONFIG_YAFFS_WINCE
	__u32 not_for_wince[5];
#else
	__u32 yst_uid;
	__u32 yst_gid;
	__u32 yst_atime;
	__u32 yst_mtime;
	__u32 yst_ctime;
#endif

	/* File size  applies to files only */
	int file_size;

	/* Equivalent object id applies to hard links only. */
	int equiv_id;

	/* Alias is for symlinks only. */
	YCHAR alias[YAFFS_MAX_ALIAS_LENGTH + 1];

	__u32 yst_rdev;		/* device stuff for block and char devices (major/min) */

#ifdef CONFIG_YAFFS_WINCE
	__u32 win_ctime[2];
	__u32 win_atime[2];
	__u32 win_mtime[2];
#else
	__u32 room_to_grow[6];

#endif
	__u32 inband_shadowed_obj_id;
	__u32 inband_is_shrink;

	__u32 reserved[2];
	int shadows_obj;	/* This object header shadows the specified object if > 0 */

	/* is_shrink applies to object headers written when we shrink the file (ie resize) */
	__u32 is_shrink;

} yaffs_obj_header;

/*--------------------------- Tnode -------------------------- */

union yaffs_tnode_union {
	union yaffs_tnode_union *internal[YAFFS_NTNODES_INTERNAL];

};

typedef union yaffs_tnode_union yaffs_tnode_t;


/*------------------------  Object -----------------------------*/
/* An object can be one of:
 * - a directory (no data, has children links
 * - a regular file (data.... not prunes :->).
 * - a symlink [symbolic link] (the alias).
 * - a hard link
 */

typedef struct {
	__u32 file_size;
	__u32 scanned_size;
	__u32 shrink_size;
	int top_level;
	yaffs_tnode_t *top;
} yaffs_file_s;

typedef struct {
	struct ylist_head children;     /* list of child links */
	struct ylist_head dirty;	/* Entry for list of dirty directories */
} yaffs_dir_s;

typedef struct {
	YCHAR *alias;
} yaffs_symlink_t;

typedef struct {
	struct yaffs_obj_s *equiv_obj;
	__u32 equiv_id;
} yaffs_hard_link_s;

typedef union {
	yaffs_file_s file_variant;
	yaffs_dir_s dir_variant;
	yaffs_symlink_t symlink_variant;
	yaffs_hard_link_s hardlink_variant;
} yaffs_obj_variant;



struct yaffs_obj_s {
	__u8 deleted:1;		/* This should only apply to unlinked files. */
	__u8 soft_del:1;	/* it has also been soft deleted */
	__u8 unlinked:1;	/* An unlinked file. The file should be in the unlinked directory.*/
	__u8 fake:1;		/* A fake object has no presence on NAND. */
	__u8 rename_allowed:1;	/* Some objects are not allowed to be renamed. */
	__u8 unlink_allowed:1;
	__u8 dirty:1;		/* the object needs to be written to flash */
	__u8 valid:1;		/* When the file system is being loaded up, this
				 * object might be created before the data
				 * is available (ie. file data records appear before the header).
				 */
	__u8 lazy_loaded:1;	/* This object has been lazy loaded and is missing some detail */

	__u8 defered_free:1;	/* For Linux kernel. Object is removed from NAND, but is
				 * still in the inode cache. Free of object is defered.
				 * until the inode is released.
				 */
	__u8 being_created:1;	/* This object is still being created so skip some checks. */
	__u8 is_shadowed:1;	/* This object is shadowed on the way to being renamed. */

	__u8 xattr_known:1;	/* We know if this has object has xattribs or not. */
	__u8 has_xattr:1;	/* This object has xattribs. Valid if xattr_known. */

	__u8 serial;		/* serial number of chunk in NAND. Cached here */
	__u16 sum;		/* sum of the name to speed searching */

	struct yaffs_dev_s *my_dev;       /* The device I'm on */

	struct ylist_head hash_link;     /* list of objects in this hash bucket */

	struct ylist_head hard_links;    /* all the equivalent hard linked objects */

	/* directory structure stuff */
	/* also used for linking up the free list */
	struct yaffs_obj_s *parent;
	struct ylist_head siblings;

	/* Where's my object header in NAND? */
	int hdr_chunk;

	int n_data_chunks;	/* Number of data chunks attached to the file. */

	__u32 obj_id;		/* the object id value */

	__u32 yst_mode;

#ifdef CONFIG_YAFFS_SHORT_NAMES_IN_RAM
	YCHAR short_name[YAFFS_SHORT_NAME_LENGTH + 1];
#endif

#ifdef CONFIG_YAFFS_WINCE
	__u32 win_ctime[2];
	__u32 win_mtime[2];
	__u32 win_atime[2];
#else
	__u32 yst_uid;
	__u32 yst_gid;
	__u32 yst_atime;
	__u32 yst_mtime;
	__u32 yst_ctime;
#endif

	__u32 yst_rdev;

	void *my_inode;

	yaffs_obj_type variant_type;

	yaffs_obj_variant variant;

};

typedef struct yaffs_obj_s yaffs_obj_t;

typedef struct {
	struct ylist_head list;
	int count;
} yaffs_obj_bucket;


/* yaffs_checkpt_obj_t holds the definition of an object as dumped
 * by checkpointing.
 */

typedef struct {
	int struct_type;
	__u32 obj_id;
	__u32 parent_id;
	int hdr_chunk;
	yaffs_obj_type variant_type:3;
	__u8 deleted:1;
	__u8 soft_del:1;
	__u8 unlinked:1;
	__u8 fake:1;
	__u8 rename_allowed:1;
	__u8 unlink_allowed:1;
	__u8 serial;

	int n_data_chunks;
	__u32 size_or_equiv_obj;
} yaffs_checkpt_obj_t;

/*--------------------- Temporary buffers ----------------
 *
 * These are chunk-sized working buffers. Each device has a few
 */

typedef struct {
	__u8 *buffer;
	int line;	/* track from whence this buffer was allocated */
	int max_line;
} yaffs_buffer_t;

/*----------------- Device ---------------------------------*/


struct yaffs_param_s {
	const YCHAR *name;

	/*
         * Entry parameters set up way early. Yaffs sets up the rest.
         * The structure should be zeroed out before use so that unused
         * and defualt values are zero.
         */

	int inband_tags;          /* Use unband tags */
	__u32 total_bytes_per_chunk; /* Should be >= 512, does not need to be a power of 2 */
	int chunks_per_block;	/* does not need to be a power of 2 */
	int spare_bytes_per_chunk;	/* spare area size */
	int start_block;		/* Start block we're allowed to use */
	int end_block;		/* End block we're allowed to use */
	int n_reserved_blocks;	/* We want this tuneable so that we can reduce */
				/* reserved blocks on NOR and RAM. */


	int n_caches;	/* If <= 0, then short op caching is disabled, else
				 * the number of short op caches (don't use too many).
                                 * 10 to 20 is a good bet.
				 */
	int use_nand_ecc;		/* Flag to decide whether or not to use NANDECC on data (yaffs1) */
	int no_tags_ecc;		/* Flag to decide whether or not to do ECC on packed tags (yaffs2) */ 

	int is_yaffs2;           /* Use yaffs2 mode on this device */

	int empty_lost_n_found;  /* Auto-empty lost+found directory on mount */

	int refresh_period;	/* How often we should check to do a block refresh */

	/* Checkpoint control. Can be set before or after initialisation */
	__u8 skip_checkpt_rd;
	__u8 skip_checkpt_wr;

	int enable_xattr;	/* Enable xattribs */

	/* NAND access functions (Must be set before calling YAFFS)*/

	int (*write_chunk_fn) (struct yaffs_dev_s *dev,
					int nand_chunk, const __u8 *data,
					const yaffs_spare *spare);
	int (*read_chunk_fn) (struct yaffs_dev_s *dev,
					int nand_chunk, __u8 *data,
					yaffs_spare *spare);
	int (*erase_fn) (struct yaffs_dev_s *dev,
					int flash_block);
	int (*initialise_flash_fn) (struct yaffs_dev_s *dev);
	int (*deinitialise_flash_fn) (struct yaffs_dev_s *dev);

#ifdef CONFIG_YAFFS_YAFFS2
	int (*write_chunk_tags_fn) (struct yaffs_dev_s *dev,
					 int nand_chunk, const __u8 *data,
					 const yaffs_ext_tags *tags);
	int (*read_chunk_tags_fn) (struct yaffs_dev_s *dev,
					  int nand_chunk, __u8 *data,
					  yaffs_ext_tags *tags);
	int (*bad_block_fn) (struct yaffs_dev_s *dev, int block_no);
	int (*query_block_fn) (struct yaffs_dev_s *dev, int block_no,
			       yaffs_block_state_t *state, __u32 *seq_number);
#endif

	/* The remove_obj_fn function must be supplied by OS flavours that
	 * need it.
         * yaffs direct uses it to implement the faster readdir.
         * Linux uses it to protect the directory during unlocking.
	 */
	void (*remove_obj_fn)(struct yaffs_obj_s *obj);

	/* Callback to mark the superblock dirty */
	void (*sb_dirty_fn)(struct yaffs_dev_s *dev);
	
	/*  Callback to control garbage collection. */
	unsigned (*gc_control)(struct yaffs_dev_s *dev);

        /* Debug control flags. Don't use unless you know what you're doing */
	int use_header_file_size;	/* Flag to determine if we should use file sizes from the header */
	int disable_lazy_load;	/* Disable lazy loading on this device */
	int wide_tnodes_disabled; /* Set to disable wide tnodes */
	int disable_soft_del;  /* yaffs 1 only: Set to disable the use of softdeletion. */
	
	int defered_dir_update; /* Set to defer directory updates */

#ifdef CONFIG_YAFFS_AUTO_UNICODE
	int auto_unicode;
#endif
	int always_check_erased; /* Force chunk erased check always on */
};

typedef struct yaffs_param_s yaffs_param_t;

struct yaffs_dev_s {
	struct yaffs_param_s param;

        /* Context storage. Holds extra OS specific data for this device */

	void *os_context;
	void *driver_context;

	struct ylist_head dev_list;

	/* Runtime parameters. Set up by YAFFS. */
	int data_bytes_per_chunk;	

        /* Non-wide tnode stuff */
	__u16 chunk_grp_bits;	/* Number of bits that need to be resolved if
                                 * the tnodes are not wide enough.
                                 */
	__u16 chunk_grp_size;	/* == 2^^chunk_grp_bits */

	/* Stuff to support wide tnodes */
	__u32 tnode_width;
	__u32 tnode_mask;
	__u32 tnode_size;

	/* Stuff for figuring out file offset to chunk conversions */
	__u32 chunk_shift; /* Shift value */
	__u32 chunk_div;   /* Divisor after shifting: 1 for power-of-2 sizes */
	__u32 chunk_mask;  /* Mask to use for power-of-2 case */



	int is_mounted;
	int read_only;
	int is_checkpointed;


	/* Stuff to support block offsetting to support start block zero */
	int internal_start_block;
	int internal_end_block;
	int block_offset;
	int chunk_offset;


	/* Runtime checkpointing stuff */
	int checkpt_page_seq;   /* running sequence number of checkpoint pages */
	int checkpt_byte_count;
	int checkpt_byte_offs;
	__u8 *checkpt_buffer;
	int checkpt_open_write;
	int blocks_in_checkpt;
	int checkpt_cur_chunk;
	int checkpt_cur_block;
	int checkpt_next_block;
	int *checkpt_block_list;
	int checkpt_max_blocks;
	__u32 checkpt_sum;
	__u32 checkpt_xor;

	int checkpoint_blocks_required; /* Number of blocks needed to store current checkpoint set */

	/* Block Info */
	yaffs_block_info_t *block_info;
	__u8 *chunk_bits;	/* bitmap of chunks in use */
	unsigned block_info_alt:1;	/* was allocated using alternative strategy */
	unsigned chunk_bits_alt:1;	/* was allocated using alternative strategy */
	int chunk_bit_stride;	/* Number of bytes of chunk_bits per block.
				 * Must be consistent with chunks_per_block.
				 */

	int n_erased_blocks;
	int alloc_block;	/* Current block being allocated off */
	__u32 alloc_page;
	int alloc_block_finder;	/* Used to search for next allocation block */

	/* Object and Tnode memory management */
	void *allocator;
	int n_obj;
	int n_tnodes;

	int n_hardlinks;

	yaffs_obj_bucket obj_bucket[YAFFS_NOBJECT_BUCKETS];
	__u32 bucket_finder;

	int n_free_chunks;

	/* Garbage collection control */
	__u32 *gc_cleanup_list;	/* objects to delete at the end of a GC. */
	__u32 n_clean_ups;

	unsigned has_pending_prioritised_gc; /* We think this device might have pending prioritised gcs */
	unsigned gc_disable;
	unsigned gc_block_finder;
	unsigned gc_dirtiest;
	unsigned gc_pages_in_use;
	unsigned gc_not_done;
	unsigned gc_block;
	unsigned gc_chunk;
	unsigned gc_skip;

	/* Special directories */
	yaffs_obj_t *root_dir;
	yaffs_obj_t *lost_n_found;

	/* Buffer areas for storing data to recover from write failures TODO
	 *      __u8            buffered_data[YAFFS_CHUNKS_PER_BLOCK][YAFFS_BYTES_PER_CHUNK];
	 *      yaffs_spare buffered_spare[YAFFS_CHUNKS_PER_BLOCK];
	 */

	int buffered_block;	/* Which block is buffered here? */
	int doing_buffered_block_rewrite;

	yaffs_cache_t *cache;
	int cache_last_use;

	/* Stuff for background deletion and unlinked files.*/
	yaffs_obj_t *unlinked_dir;	/* Directory where unlinked and deleted files live. */
	yaffs_obj_t *del_dir;	/* Directory where deleted objects are sent to disappear. */
	yaffs_obj_t *unlinked_deletion;	/* Current file being background deleted.*/
	int n_deleted_files;		/* Count of files awaiting deletion;*/
	int n_unlinked_files;		/* Count of unlinked files. */
	int n_bg_deletions;	/* Count of background deletions. */

	/* Temporary buffer management */
	yaffs_buffer_t temp_buffer[YAFFS_N_TEMP_BUFFERS];
	int max_temp;
	int temp_in_use;
	int unmanaged_buffer_allocs;
	int unmanaged_buffer_deallocs;

	/* yaffs2 runtime stuff */
	unsigned seq_number;	/* Sequence number of currently allocating block */
	unsigned oldest_dirty_seq;
	unsigned oldest_dirty_block;

	/* Block refreshing */
	int refresh_skip;	/* A skip down counter. Refresh happens when this gets to zero. */

	/* Dirty directory handling */
	struct ylist_head dirty_dirs; /* List of dirty directories */


	/* Statistcs */
	__u32 n_page_writes;
	__u32 n_page_reads;
	__u32 n_erasures;
	__u32 n_erase_failures;
	__u32 n_gc_copies;
	__u32 all_gcs;
	__u32 passive_gc_count;
	__u32 oldest_dirty_gc_count;
	__u32 n_gc_blocks;
	__u32 bg_gcs;
	__u32 n_retired_writes;
	__u32 n_retired_blocks;
	__u32 n_ecc_fixed;
	__u32 n_ecc_unfixed;
	__u32 n_tags_ecc_fixed;
	__u32 n_tags_ecc_unfixed;
	__u32 n_deletions;
	__u32 n_unmarked_deletions;
	__u32 refresh_count;
	__u32 cache_hits;

};

typedef struct yaffs_dev_s yaffs_dev_t;

/* The static layout of block usage etc is stored in the super block header */
typedef struct {
	int StructType;
	int version;
	int checkpt_start_block;
	int checkpt_end_block;
	int start_block;
	int end_block;
	int rfu[100];
} yaffs_sb_header;

/* The CheckpointDevice structure holds the device information that changes at runtime and
 * must be preserved over unmount/mount cycles.
 */
typedef struct {
	int struct_type;
	int n_erased_blocks;
	int alloc_block;	/* Current block being allocated off */
	__u32 alloc_page;
	int n_free_chunks;

	int n_deleted_files;		/* Count of files awaiting deletion;*/
	int n_unlinked_files;		/* Count of unlinked files. */
	int n_bg_deletions;	/* Count of background deletions. */

	/* yaffs2 runtime stuff */
	unsigned seq_number;	/* Sequence number of currently allocating block */

} yaffs_checkpt_dev_t;


typedef struct {
	int struct_type;
	__u32 magic;
	__u32 version;
	__u32 head;
} yaffs_checkpt_validty_t;


struct yaffs_shadow_fixer_s {
	int obj_id;
	int shadowed_id;
	struct yaffs_shadow_fixer_s *next;
};

/* Structure for doing xattr modifications */
typedef struct {
	int set; /* If 0 then this is a deletion */
	const YCHAR *name;
	const void *data;
	int size;
	int flags;
	int result;
}yaffs_xattr_mod;


/*----------------------- YAFFS Functions -----------------------*/

int yaffs_guts_initialise(yaffs_dev_t *dev);
void yaffs_deinitialise(yaffs_dev_t *dev);

int yaffs_get_n_free_chunks(yaffs_dev_t *dev);

int yaffs_rename_obj(yaffs_obj_t *old_dir, const YCHAR *old_name,
		       yaffs_obj_t *new_dir, const YCHAR *new_name);

int yaffs_unlinker(yaffs_obj_t *dir, const YCHAR *name);
int yaffs_del_obj(yaffs_obj_t *obj);

int yaffs_get_obj_name(yaffs_obj_t *obj, YCHAR *name, int buffer_size);
int yaffs_get_obj_length(yaffs_obj_t *obj);
int yaffs_get_obj_inode(yaffs_obj_t *obj);
unsigned yaffs_get_obj_type(yaffs_obj_t *obj);
int yaffs_get_obj_link_count(yaffs_obj_t *obj);

int yaffs_set_attribs(yaffs_obj_t *obj, struct iattr *attr);
int yaffs_get_attribs(yaffs_obj_t *obj, struct iattr *attr);

/* File operations */
int yaffs_file_rd(yaffs_obj_t *obj, __u8 *buffer, loff_t offset,
				int n_bytes);
int yaffs_wr_file(yaffs_obj_t *obj, const __u8 *buffer, loff_t offset,
				int n_bytes, int write_trhrough);
int yaffs_resize_file(yaffs_obj_t *obj, loff_t new_size);

yaffs_obj_t *yaffs_create_file(yaffs_obj_t *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid);

int yaffs_flush_file(yaffs_obj_t *obj, int update_time, int data_sync);

/* Flushing and checkpointing */
void yaffs_flush_whole_cache(yaffs_dev_t *dev);

int yaffs_checkpoint_save(yaffs_dev_t *dev);
int yaffs_checkpoint_restore(yaffs_dev_t *dev);

/* Directory operations */
yaffs_obj_t *yaffs_create_dir(yaffs_obj_t *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid);
yaffs_obj_t *yaffs_find_by_name(yaffs_obj_t *the_dir, const YCHAR *name);
int yaffs_ApplyToDirectoryChildren(yaffs_obj_t *the_dir,
				   int (*fn) (yaffs_obj_t *));

yaffs_obj_t *yaffs_find_by_number(yaffs_dev_t *dev, __u32 number);

/* Link operations */
yaffs_obj_t *yaffs_link_obj(yaffs_obj_t *parent, const YCHAR *name,
			 yaffs_obj_t *equiv_obj);

yaffs_obj_t *yaffs_get_equivalent_obj(yaffs_obj_t *obj);

/* Symlink operations */
yaffs_obj_t *yaffs_create_symlink(yaffs_obj_t *parent, const YCHAR *name,
				 __u32 mode, __u32 uid, __u32 gid,
				 const YCHAR *alias);
YCHAR *yaffs_get_symlink_alias(yaffs_obj_t *obj);

/* Special inodes (fifos, sockets and devices) */
yaffs_obj_t *yaffs_create_special(yaffs_obj_t *parent, const YCHAR *name,
				 __u32 mode, __u32 uid, __u32 gid, __u32 rdev);


int yaffs_set_xattrib(yaffs_obj_t *obj, const YCHAR *name, const void * value, int size, int flags);
int yaffs_get_xattrib(yaffs_obj_t *obj, const YCHAR *name, void *value, int size);
int yaffs_list_xattrib(yaffs_obj_t *obj, char *buffer, int size);
int yaffs_remove_xattrib(yaffs_obj_t *obj, const YCHAR *name);

/* Special directories */
yaffs_obj_t *yaffs_root(yaffs_dev_t *dev);
yaffs_obj_t *yaffs_lost_n_found(yaffs_dev_t *dev);

#ifdef CONFIG_YAFFS_WINCE
/* CONFIG_YAFFS_WINCE special stuff */
void yfsd_win_file_time_now(__u32 target[2]);
#endif

void yaffs_handle_defered_free(yaffs_obj_t *obj);

void yaffs_update_dirty_dirs(yaffs_dev_t *dev);

int yaffs_bg_gc(yaffs_dev_t *dev, unsigned urgency);

/* Debug dump  */
int yaffs_dump_obj(yaffs_obj_t *obj);

void yaffs_guts_test(yaffs_dev_t *dev);

/* A few useful functions to be used within the core files*/
void yaffs_chunk_del(yaffs_dev_t *dev, int chunk_id, int mark_flash, int lyn);
int yaffs_check_ff(__u8 *buffer, int n_bytes);
void yaffs_handle_chunk_error(yaffs_dev_t *dev, yaffs_block_info_t *bi);

__u8 *yaffs_get_temp_buffer(yaffs_dev_t *dev, int line_no);
void yaffs_release_temp_buffer(yaffs_dev_t *dev, __u8 *buffer, int line_no);

yaffs_obj_t *yaffs_find_or_create_by_number(yaffs_dev_t *dev,
					        int number,
					        yaffs_obj_type type);
int yaffs_put_chunk_in_file(yaffs_obj_t *in, int inode_chunk,
			        int nand_chunk, int in_scan);
void yaffs_set_obj_name(yaffs_obj_t *obj, const YCHAR *name);
void yaffs_set_obj_name_from_oh(yaffs_obj_t *obj, const yaffs_obj_header *oh);
void yaffs_add_obj_to_dir(yaffs_obj_t *directory,
					yaffs_obj_t *obj);
YCHAR *yaffs_clone_str(const YCHAR *str);
void yaffs_link_fixup(yaffs_dev_t *dev, yaffs_obj_t *hard_list);
void yaffs_block_became_dirty(yaffs_dev_t *dev, int block_no);
int yaffs_update_oh(yaffs_obj_t *in, const YCHAR *name,
				int force, int is_shrink, int shadows,
                                yaffs_xattr_mod *xop);
void yaffs_handle_shadowed_obj(yaffs_dev_t *dev, int obj_id,
				int backward_scanning);
int yaffs_check_alloc_available(yaffs_dev_t *dev, int n_chunks);
yaffs_tnode_t *yaffs_get_tnode(yaffs_dev_t *dev);
yaffs_tnode_t *yaffs_add_find_tnode_0(yaffs_dev_t *dev,
					yaffs_file_s *file_struct,
					__u32 chunk_id,
					yaffs_tnode_t *passed_tn);

int yaffs_do_file_wr(yaffs_obj_t *in, const __u8 *buffer, loff_t offset,
			int n_bytes, int write_trhrough);
void yaffs_resize_file_down( yaffs_obj_t *obj, loff_t new_size);
void yaffs_skip_rest_of_block(yaffs_dev_t *dev);

int yaffs_count_free_chunks(yaffs_dev_t *dev);

yaffs_tnode_t *yaffs_find_tnode_0(yaffs_dev_t *dev,
				yaffs_file_s *file_struct,
				__u32 chunk_id);

__u32 yaffs_get_group_base(yaffs_dev_t *dev, yaffs_tnode_t *tn, unsigned pos);

#endif
