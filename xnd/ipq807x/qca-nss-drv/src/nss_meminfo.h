/*
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * nss_meminfo.h
 *	nss meminfo header file.
 */

#ifndef __NSS_MEMINFO_H
#define __NSS_MEMINFO_H

#define NSS_MEMINFO_RESERVE_AREA_SIZE	0x1000	/* Size of reserved space in firmware start code aligned to one page */
#define NSS_MEMINFO_RESERVE_AREA_MAGIC	0x9526	/* Magic at the beginning of reserved space */
#define NSS_MEMINFO_MAP_START_OFFSET	8	/* Offset of memory map start address in reserved space */
#define NSS_MEMINFO_MAP_SIZE		0x1000	/* Size of memory map per core aligned to one page */
#define NSS_MEMINFO_MAP_START_MAGIC	0x9527
#define NSS_MEMINFO_REQUEST_MAGIC	0X9528
#define NSS_MEMINFO_MAP_END_MAGIC	0x9529
#define NSS_MEMINFO_RESERVE_AREA_UTCM_SHARED_MAP_MAGIC 	0x9530 /* Magic at the beginning of UTCM_SHARED reserved space */
#define NSS_MEMINFO_BLOCK_NAME_MAXLEN	48
#define NSS_MEMINFO_MEMTYPE_NAME_MAXLEN	32
#define NSS_MEMINFO_USER_CONFIG_MAXLEN	1024
#define NSS_MEMINFO_POISON 		0x95	/* Invalid kernel memory address assigned for non mapable mem types */

/*
 * Memory types available
 */
enum nss_meminfo_memtype {
	NSS_MEMINFO_MEMTYPE_IMEM,	/* NSS-IMEM also called TCM */
	NSS_MEMINFO_MEMTYPE_SDRAM,	/* SDRAM also called DDR */
	NSS_MEMINFO_MEMTYPE_UTCM_SHARED, /* UTCM memory allocated for DMA objects */
	NSS_MEMINFO_MEMTYPE_INFO,	/* Exchange information during boot up */
	NSS_MEMINFO_MEMTYPE_MAX
};

/*
 * Memory request
 * Firmware package defines each request asking host to feed the request.
 */
struct nss_meminfo_request {
	uint16_t magic;					/* Request magic */
	char name[NSS_MEMINFO_BLOCK_NAME_MAXLEN];	/* Memory block name */
	uint16_t memtype_default;			/* Memory type requested */
	uint16_t memtype_user;				/* User-defined memory type */
	uint32_t alignment;				/* Alignment requirement */
	uint32_t size;					/* Size requested */
	uint32_t addr;					/* Memory block address got from host */
};

/*
 * Memory map
 * It starts with a magic then an array of memory request and end with a checksum.
 * Firmware creates the map for host to parse.
 */
struct nss_meminfo_map {
	uint32_t *start;				/* Start address */
	uint32_t num_requests;				/* Number of requests */
	struct nss_meminfo_request *requests;		/* Start of Request array */
};

/*
 * Memory block
 * Block node for each request.
 */
struct nss_meminfo_block {
	struct nss_meminfo_block *next;	/* Next block in the same list */
	uint32_t index;			/* Index to request array */
	uint32_t size;			/* Size of memory block */
	uint32_t dma_addr;		/* DMA address */
	void *kern_addr;		/* Kernel address */
};

/*
 * Memory block list
 * List of block node of same memory type.
 */
struct nss_meminfo_block_list {
	enum nss_meminfo_memtype memtype;	/* memory type */
	uint32_t num_blks;			/* Number of blocks */
	uint32_t total_size;			/* Size of all memory blocks in this list */
	struct nss_meminfo_block *head;		/* list head */
};

/*
 * H2N/N2H rings information
 */
struct nss_meminfo_n2h_h2n_info {
	enum nss_meminfo_memtype memtype;	/* Memory type */
	uint32_t total_size;			/* Total size */
	uint32_t dma_addr;			/* DMA address */
	void *kern_addr;			/* Kernel address */
};

/*
 * Memory context
 */
struct nss_meminfo_ctx {
	struct nss_meminfo_n2h_h2n_info n2h_info;	/* N2H rings info*/
	struct nss_meminfo_n2h_h2n_info h2n_info;	/* H2N rings info */
	uint32_t imem_head;				/* IMEM start address */
	uint32_t imem_end;				/* IMEM end address */
	uint32_t imem_tail;				/* IMEM data end */
	uint32_t utcm_shared_head;			/* UTCM_SHARED start address */
	uint32_t utcm_shared_end;			/* UTCM_SHARED end address */
	uint32_t utcm_shared_tail;			/* UTCM_SHARED data end */
	struct nss_if_mem_map *if_map;			/* nss_if_mem_map_inst virtual address */
	uint32_t if_map_dma;				/* nss_if_mem_map_inst physical address */
	enum nss_meminfo_memtype if_map_memtype;	/* Memory type for nss_if_mem_map */
	struct nss_log_descriptor *logbuffer;		/* nss_logbuffer virtual address */
	uint32_t logbuffer_dma;				/* nss_logbuffer physical address */
	enum nss_meminfo_memtype logbuffer_memtype;	/* Memory type for logbuffer */
	uint32_t c2c_start_dma;				/* nss_c2c start physical address */
	enum nss_meminfo_memtype c2c_start_memtype;	/* Memory type for c2c_start */
	void *sdma_ctrl;				/* Soft DMA controller */

	struct nss_meminfo_map meminfo_map;		/* Meminfo map */
	struct nss_meminfo_block_list block_lists[NSS_MEMINFO_MEMTYPE_MAX];
							/* Block lists for each memory type */
};

bool nss_meminfo_init(struct nss_ctx_instance *nss_ctx);
#endif
