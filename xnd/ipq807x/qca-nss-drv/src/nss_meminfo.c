/*
 * Copyright (c) 2018-2021, The Linux Foundation. All rights reserved.
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
 * nss_meminfo.c
 *	NSS meminfo subsystem
 */

#include <linux/seq_file_net.h>
#include "nss_tx_rx_common.h"
#include "nss_core.h"
#include "nss_arch.h"
#include "nss_meminfo.h"

/*
 * Store user configuration
 */
static char nss_meminfo_user_config[NSS_MEMINFO_USER_CONFIG_MAXLEN];
module_param_string(meminfo_user_config, nss_meminfo_user_config,
		    NSS_MEMINFO_USER_CONFIG_MAXLEN, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(nss_meminfo_user_config, "meminfo user configuration");

static bool nss_meminfo_debugfs_exist;

/*
 * Name table of memory type presented to user.
 */
char *nss_meminfo_memtype_table[NSS_MEMINFO_MEMTYPE_MAX] = {"IMEM", "SDRAM", "UTCM_SHARED"};

/*
 * nss_meminfo_alloc_sdram()
 *	Allocate a SDRAM block.
 */
static void *nss_meminfo_alloc_sdram(struct nss_ctx_instance *nss_ctx, uint32_t size)
{
	void *addr = 0;

	/*
	 * kmalloc() return cache line aligned buffer.
	 */
	addr = kmalloc(size, GFP_KERNEL | __GFP_ZERO);
	if (!addr)
		nss_info_always("%px: failed to alloc a sdram block of size %u\n", nss_ctx, size);

	kmemleak_not_leak((void *)addr);
	return addr;
}

/*
 * nss_meminfo_free_sdram()
 *	Free SDRAM memory.
 */
static inline void nss_meminfo_free_sdram(struct nss_ctx_instance *nss_ctx, uint32_t dma_addr,
						void *kern_addr, uint32_t size)
{
	/*
	 * Unmap it since every SDRAM memory had been mapped.
	 */
	dma_unmap_single(nss_ctx->dev, dma_addr, size, DMA_FROM_DEVICE);
	kfree(kern_addr);
}

/*
 * nss_meminfo_alloc_imem()
 *	Allocate an IMEM block in a sequential way.
 */
static uint32_t nss_meminfo_alloc_imem(struct nss_ctx_instance *nss_ctx, uint32_t size, int alignment)
{
	struct nss_meminfo_ctx *mem_ctx = &nss_ctx->meminfo_ctx;
	uint32_t new_tail;
	uint32_t addr = 0;
	int mask;

	mask = alignment - 1;

	/*
	 * Alignment has to be a power of 2.
	 */
	nss_assert(!(alignment & mask));

	new_tail = mem_ctx->imem_tail;

	/*
	 * Align up the address if it not aligned.
	 */
	if (new_tail & mask)
		new_tail = (new_tail + mask) & ~mask;

	if (size > (mem_ctx->imem_end - new_tail)) {
		nss_info_always("%px: failed to alloc an IMEM block of size %u\n", nss_ctx, size);
		return addr;
	}

	addr = new_tail;
	mem_ctx->imem_tail = new_tail + size;

	return addr;
}

/*
 * nss_meminfo_free_imem()
 *	Free an IMEM block. Ignore the padding bytes for alignment requirement.
 */
static void nss_meminfo_free_imem(struct nss_ctx_instance *nss_ctx, uint32_t addr, uint32_t size)
{
	struct nss_meminfo_ctx *mem_ctx = &nss_ctx->meminfo_ctx;
	mem_ctx->imem_tail -= size;
}

/*
 * nss_meminfo_alloc_utcm_shared()
 *	Allocate an UTCM_SHARED block in a sequential way.
 */
static uint32_t nss_meminfo_alloc_utcm_shared(struct nss_ctx_instance *nss_ctx, uint32_t size, int alignment)
{
	struct nss_meminfo_ctx *mem_ctx = &nss_ctx->meminfo_ctx;
	uint32_t new_tail;
	uint32_t addr = 0;
	int mask;

	mask = alignment - 1;

	/*
	 * Alignment has to be a power of 2.
	 */
	nss_assert(!(alignment & mask));

	new_tail = mem_ctx->utcm_shared_tail;

	/*
	 * Align up the address if it not aligned.
	 */
	if (new_tail & mask)
		new_tail = (new_tail + mask) & ~mask;

	if (size > (mem_ctx->utcm_shared_end - new_tail)) {
		nss_info_always("%px: failed to alloc an UTCM_SHARED block of size %u\n", nss_ctx, size);
		return addr;
	}

	addr = new_tail;
	mem_ctx->utcm_shared_tail = new_tail + size;

	return addr;
}

/*
 * nss_meminfo_free_utcm_shared()
 *	Free an UTCM_SHARED block. Ignore the padding bytes for alignment requirement.
 */
static void nss_meminfo_free_utcm_shared(struct nss_ctx_instance *nss_ctx, uint32_t addr, uint32_t size)
{
	struct nss_meminfo_ctx *mem_ctx = &nss_ctx->meminfo_ctx;
	mem_ctx->utcm_shared_tail -= size;
}

/*
 * nss_meminfo_if_user_overwrite()
 *	Return user configured memory type. Otherwise, return -1.
 */
static int nss_meminfo_if_user_overwrite(struct nss_ctx_instance *nss_ctx, const char *name)
{
	char *user_config;
	char **mtype_table;
	char needle[NSS_MEMINFO_BLOCK_NAME_MAXLEN + 6];
	char user_choice[NSS_MEMINFO_MEMTYPE_NAME_MAXLEN];
	int i;
	char *p;

	user_config = nss_meminfo_user_config;
	mtype_table = nss_meminfo_memtype_table;

	snprintf(needle, sizeof(needle), "<%1d, %s, ", nss_ctx->id, name);

	p = strstr(user_config, needle);
	if (!p)
		return -1;

	p += strlen(needle);

	for (i = 0; i < NSS_MEMINFO_MEMTYPE_NAME_MAXLEN - 1; i++) {
		/*
		 * Each user config is like <core_id, object_name, memory_type>,
		 * it starts with '<' and ends with '>'.
		 */
		if (*p == '>' || *p == '\0')
			break;
		user_choice[i] = *p;
		p++;
	}

	user_choice[i] = '\0';

	for (i = 0; i < NSS_MEMINFO_MEMTYPE_MAX; i++)
		if (!strcmp(mtype_table[i], user_choice))
			return i;

	return -1;
}

/*
 * nss_meminfo_free_block_lists()
 *	Free block node and memory associated with each each memory object.
 */
static void nss_meminfo_free_block_lists(struct nss_ctx_instance *nss_ctx)
{
	struct nss_meminfo_ctx *mem_ctx;
	struct nss_meminfo_block_list *l;
	int i;

	mem_ctx = &nss_ctx->meminfo_ctx;
	for (i = 0; i < NSS_MEMINFO_MEMTYPE_MAX; i++) {
		struct nss_meminfo_block *b;
		l = &mem_ctx->block_lists[i];
		b = l->head;
		while (b) {
			struct nss_meminfo_block *tmp;
			/*
			 * Free IMEM/SDRAM/UTCM_SHARED memory.
			 */
			switch (i) {
			case NSS_MEMINFO_MEMTYPE_IMEM:
				nss_meminfo_free_imem(nss_ctx, b->dma_addr, b->size);
				break;
			case NSS_MEMINFO_MEMTYPE_SDRAM:
				nss_meminfo_free_sdram(nss_ctx, b->dma_addr, b->kern_addr, b->size);
				break;
			case NSS_MEMINFO_MEMTYPE_UTCM_SHARED:
				nss_meminfo_free_utcm_shared(nss_ctx, b->dma_addr, b->size);
				break;
			}

			/*
			 * Free the struct nss_meminfo_block itself.
			 */
			tmp = b;
			b = b->next;
			kfree(tmp);
		}
	}
}

/*
 * nss_meminfo_init_block_lists()
 *	Initialize block lists and allocate memory for each block.
 */
static bool nss_meminfo_init_block_lists(struct nss_ctx_instance *nss_ctx)
{
	/*
	 * There is no corresponding mapped address in kernel for UTCM_SHARED.
	 * UTCM_SHARED access from kernel is not allowed. Mem Objects requesting
	 * UTCM_SHARED are not expected to use any kernel mapped address.
	 * Was for UTCM_SHARED, but move to here as default especially for KW scan.
	 * Thus, NSS_MEMINFO_POISON is the default value for non-mappable memory request.
	 */
	void *kern_addr = (void *)NSS_MEMINFO_POISON;
	uint32_t dma_addr = 0;
	struct nss_meminfo_ctx *mem_ctx;
	struct nss_meminfo_block_list *l;
	struct nss_meminfo_request *r;
	struct nss_meminfo_map *map;
	int mtype;
	int i;

	mem_ctx = &nss_ctx->meminfo_ctx;

	/*
	 * Fill memory type for each block list.
	 */
	for (i = 0; i < NSS_MEMINFO_MEMTYPE_MAX; i++)
		mem_ctx->block_lists[i].memtype = i;

	map = &mem_ctx->meminfo_map;

	/*
	 * Loop through all meminfo requests by checking the per-request magic.
	 */
	for (r = map->requests; r->magic == NSS_MEMINFO_REQUEST_MAGIC; r++) {
		struct nss_meminfo_block *b = (struct nss_meminfo_block *)
						kmalloc(sizeof(struct nss_meminfo_block), GFP_KERNEL);
		if (!b) {
			nss_info_always("%px: failed to allocate meminfo block\n", nss_ctx);
			goto cleanup;
		}

		b->index = map->num_requests++;
		b->size = r->size;

		/*
		 * Look up the user-defined memory type.
		 * Return user-defined memory type if exists. Otherwise, return -1.
		 */
		mtype = nss_meminfo_if_user_overwrite(nss_ctx, r->name);
		if (mtype == -1)
			mtype = r->memtype_default;
		r->memtype_user = mtype;

		switch (mtype) {
		case NSS_MEMINFO_MEMTYPE_IMEM:
			/*
			 * For SOC's where TCM is not present
			 */
			if (!nss_ctx->vphys) {
				nss_info_always("%px:IMEM requested but TCM not defined "
								"for this SOC\n", nss_ctx);
				goto cleanup;
			}

			/*
			 * Return SoC real address for IMEM as DMA address.
			 */
			dma_addr = nss_meminfo_alloc_imem(nss_ctx, r->size, r->alignment);
			if (!dma_addr) {
				nss_info_always("%px: failed to alloc IMEM block\n", nss_ctx);
				goto cleanup;
			}

			/*
			 * Calulate offset to the kernel address (vmap) where the
			 * whole IMEM is mapped onto instead of calling ioremap().
			 */
			kern_addr = nss_ctx->vmap + dma_addr - nss_ctx->vphys;
			break;
		case NSS_MEMINFO_MEMTYPE_SDRAM:
			kern_addr = nss_meminfo_alloc_sdram(nss_ctx, r->size);
			if (!kern_addr) {
				nss_info_always("%px: failed to alloc SDRAM block\n", nss_ctx);
				goto cleanup;
			}

			dma_addr = dma_map_single(nss_ctx->dev, kern_addr, r->size, DMA_TO_DEVICE);
			if (unlikely(dma_mapping_error(nss_ctx->dev, dma_addr))) {
				nss_info_always("%px: failed to map SDRAM block\n", nss_ctx);
				goto cleanup;
			}
			break;
		case NSS_MEMINFO_MEMTYPE_UTCM_SHARED:
			/*
			 * Return SoC real address for UTCM_SHARED as DMA address.
			 */
			dma_addr = nss_meminfo_alloc_utcm_shared(nss_ctx, r->size, r->alignment);
			if (!dma_addr) {
				nss_info_always("%px: failed to alloc UTCM_SHARED block\n", nss_ctx);
				goto cleanup;
			}
			break;
		case NSS_MEMINFO_MEMTYPE_INFO:
			/*
			 * if FW request heap_ddr_size, fill it in from DTS values.
			 */
			if (!strcmp(r->name, "heap_ddr_size")) {
				struct nss_mmu_ddr_info coreinfo;
				r->size = nss_core_ddr_info(&coreinfo);

				/*
				 * split memory among the number of cores
				 */
				r->size /= coreinfo.num_active_cores;
				dma_addr = coreinfo.start_address + nss_ctx->id * r->size;
				nss_info_always("%px: NSS core %d DDR from %x to %x\n", nss_ctx,
						nss_ctx->id, dma_addr, dma_addr + r->size);
			}
			break;
		default:
			nss_info_always("%px: %d unsupported memory type\n", nss_ctx, mtype);
			goto cleanup;
		}

		/*
		 * Update the request with DMA address for the memory that only be used by FW.
		 */
		r->addr = dma_addr;

		/*
		 * nss_if_mem_map settings
		 */
		if (!strcmp(r->name, "nss_if_mem_map_inst")) {
			BUG_ON(mtype == NSS_MEMINFO_MEMTYPE_UTCM_SHARED);
			mem_ctx->if_map_memtype = mtype;
			mem_ctx->if_map_dma = dma_addr;
			mem_ctx->if_map = (struct nss_if_mem_map *)kern_addr;
		}

		if (!strcmp(r->name, "debug_boot_log_desc")) {
			BUG_ON(mtype == NSS_MEMINFO_MEMTYPE_UTCM_SHARED);
			mem_ctx->logbuffer_memtype = mtype;
			mem_ctx->logbuffer_dma = dma_addr;
			mem_ctx->logbuffer = (struct nss_log_descriptor *)kern_addr;
		}

		if (!strcmp(r->name, "c2c_descs_if_mem_map")) {
			mem_ctx->c2c_start_memtype = mtype;
			mem_ctx->c2c_start_dma = dma_addr;
		}

		if (strcmp(r->name, "profile_dma_ctrl") == 0) {
			mem_ctx->sdma_ctrl = kern_addr;
		nss_info_always("%px: set sdma %px\n", nss_ctx, kern_addr);
		}

		/*
		 * Flush the updated meminfo request.
		 */
		NSS_CORE_DMA_CACHE_MAINT(r, sizeof(struct nss_meminfo_request), DMA_TO_DEVICE);
		NSS_CORE_DSB();

		/*
		 * Update the list
		 */
		l = &mem_ctx->block_lists[mtype];
		l->num_blks++;
		l->total_size += r->size;

		b->next = l->head;
		l->head = b;
	}

	/*
	 * Verify memory map end magic
	 */
	if (*((uint16_t *)r) != NSS_MEMINFO_MAP_END_MAGIC)
		goto cleanup;

	return true;

cleanup:
	nss_meminfo_free_block_lists(nss_ctx);
	return false;
}

/*
 * nss_meminfo_allocate_n2h_h2n_rings()
 *	Allocate N2H/H2N rings.
 */
static bool nss_meminfo_allocate_n2h_h2n_rings(struct nss_ctx_instance *nss_ctx,
						struct nss_meminfo_n2h_h2n_info *info)
{
	switch (info->memtype) {
	case NSS_MEMINFO_MEMTYPE_SDRAM:
		info->kern_addr = nss_meminfo_alloc_sdram(nss_ctx, info->total_size);
		if (!info->kern_addr)
			return false;

		info->dma_addr = dma_map_single(nss_ctx->dev, (void *)info->kern_addr,
						info->total_size, DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(nss_ctx->dev, info->dma_addr))) {
			kfree((void *)info->kern_addr);
			return false;
		}
		break;
	case NSS_MEMINFO_MEMTYPE_IMEM:
		/*
		 * For SOC's where TCM is not present
		 */
		if (!nss_ctx->vphys) {
			nss_info_always("%px:IMEM requested but TCM not defined "
							"for this SOC\n", nss_ctx);
			return false;
		}

		info->dma_addr = nss_meminfo_alloc_imem(nss_ctx, info->total_size, L1_CACHE_BYTES);
		if (!info->dma_addr)
			return false;

		info->kern_addr = nss_ctx->vmap + info->dma_addr - nss_ctx->vphys;
		break;
	default:
		return false;
	}

	return true;
}

/*
 * nss_meminfo_configure_n2h_h2n_rings()
 *	Configure N2H/H2N rings and if_map.
 */
static bool nss_meminfo_configure_n2h_h2n_rings(struct nss_ctx_instance *nss_ctx)
{
	struct nss_meminfo_ctx *mem_ctx = &nss_ctx->meminfo_ctx;
	struct nss_meminfo_n2h_h2n_info *h2n_info;
	struct nss_meminfo_n2h_h2n_info *n2h_info;
	struct nss_if_mem_map *if_map;
	int i;
	int mtype;

	h2n_info = &mem_ctx->h2n_info;
	n2h_info = &mem_ctx->n2h_info;

	/*
	 * Check memory type. SDRAM is the default option.
	 */
	mtype = nss_meminfo_if_user_overwrite(nss_ctx, "h2n_rings");
	if (mtype == -1)
		mtype = NSS_MEMINFO_MEMTYPE_SDRAM;

	h2n_info->memtype = mtype;

	mtype = nss_meminfo_if_user_overwrite(nss_ctx, "n2h_rings");
	if (mtype == -1)
		mtype = NSS_MEMINFO_MEMTYPE_SDRAM;

	n2h_info->memtype = mtype;

	n2h_info->total_size = sizeof(struct n2h_descriptor) * NSS_N2H_RING_COUNT * (NSS_RING_SIZE + 2);
	h2n_info->total_size = sizeof(struct h2n_descriptor) * NSS_H2N_RING_COUNT * (NSS_RING_SIZE + 2);

	/*
	 * N2H ring allocations
	 */
	if (!(nss_meminfo_allocate_n2h_h2n_rings(nss_ctx, n2h_info))) {
		nss_info_always("%px: failed to allocate/map n2h rings\n", nss_ctx);
		return false;
	}

	/*
	 * H2N ring allocations
	 */
	if (!(nss_meminfo_allocate_n2h_h2n_rings(nss_ctx, h2n_info))) {
		nss_info_always("%px: failed to allocate/map h2n_rings\n", nss_ctx);
		goto cleanup;
	}

	/*
	 * Bring a fresh copy of if_map from memory in order to read it correctly.
	 */
	if_map = mem_ctx->if_map;
	NSS_CORE_DMA_CACHE_MAINT((void *)if_map, sizeof(struct nss_if_mem_map), DMA_FROM_DEVICE);
	NSS_CORE_DSB();

	if_map->n2h_rings = NSS_N2H_RING_COUNT;
	if_map->h2n_rings = NSS_H2N_RING_COUNT;

	/*
	 * N2H ring settings
	 */
	for (i = 0; i < NSS_N2H_RING_COUNT; i++) {
		struct hlos_n2h_desc_ring *n2h_desc_ring = &nss_ctx->n2h_desc_ring[i];
		n2h_desc_ring->desc_ring.desc = (struct n2h_descriptor *)(n2h_info->kern_addr + i * sizeof(struct n2h_descriptor) * (NSS_RING_SIZE + 2));
		n2h_desc_ring->desc_ring.size = NSS_RING_SIZE;
		n2h_desc_ring->hlos_index = if_map->n2h_hlos_index[i];

		if_map->n2h_desc_if[i].size = NSS_RING_SIZE;
		if_map->n2h_desc_if[i].desc_addr = n2h_info->dma_addr + i * sizeof(struct n2h_descriptor) * (NSS_RING_SIZE + 2);
		nss_info("%px: N2H ring %d, size %d, addr = %x\n", nss_ctx, i, if_map->n2h_desc_if[i].size, if_map->n2h_desc_if[i].desc_addr);
	}

	/*
	 * H2N ring settings
	 */
	for (i = 0; i < NSS_H2N_RING_COUNT; i++) {
		struct hlos_h2n_desc_rings *h2n_desc_ring = &nss_ctx->h2n_desc_rings[i];
		h2n_desc_ring->desc_ring.desc = (struct h2n_descriptor *)(h2n_info->kern_addr + i * sizeof(struct h2n_descriptor) * (NSS_RING_SIZE + 2));
		h2n_desc_ring->desc_ring.size = NSS_RING_SIZE;
		h2n_desc_ring->hlos_index = if_map->h2n_hlos_index[i];
		spin_lock_init(&h2n_desc_ring->lock);

		if_map->h2n_desc_if[i].size = NSS_RING_SIZE;
		if_map->h2n_desc_if[i].desc_addr = h2n_info->dma_addr + i * sizeof(struct h2n_descriptor) * (NSS_RING_SIZE + 2);
		nss_info("%px: H2N ring %d, size %d, addr = %x\n", nss_ctx, i, if_map->h2n_desc_if[i].size, if_map->h2n_desc_if[i].desc_addr);
	}

	/*
	 * Flush the updated nss_if_mem_map.
	 */
	NSS_CORE_DMA_CACHE_MAINT((void *)if_map, sizeof(struct nss_if_mem_map), DMA_TO_DEVICE);
	NSS_CORE_DSB();

	return true;

cleanup:
	if (n2h_info->memtype == NSS_MEMINFO_MEMTYPE_SDRAM)
		nss_meminfo_free_sdram(nss_ctx, n2h_info->dma_addr, n2h_info->kern_addr, n2h_info->total_size);
	else
		nss_meminfo_free_imem(nss_ctx, n2h_info->dma_addr, n2h_info->total_size);

	nss_meminfo_free_block_lists(nss_ctx);
	return false;
}

/*
 * nss_meminfo_config_show()
 *	function to show meinfo configuration per core.
 */
static int nss_meminfo_config_show(struct seq_file *seq, void *v)
{
	struct nss_ctx_instance *nss_ctx;
	struct nss_meminfo_ctx *mem_ctx;
	struct nss_meminfo_n2h_h2n_info *n2h_info;
	struct nss_meminfo_n2h_h2n_info *h2n_info;
	struct nss_meminfo_map *map;
	struct nss_meminfo_request *r;
	int nss_id;
	int i;

	/*
	 * i_private is passed to us by debug_fs_create()
	 */
	nss_id = (int)(nss_ptr_t)seq->private;
	if (nss_id < 0 || nss_id >= nss_top_main.num_nss) {
		nss_warning("nss_id: %d is not valid\n", nss_id);
		return -ENODEV;
	}

	nss_ctx = &nss_top_main.nss[nss_id];
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	mem_ctx = &nss_ctx->meminfo_ctx;
	map = &mem_ctx->meminfo_map;
	n2h_info = &mem_ctx->n2h_info;
	h2n_info = &mem_ctx->h2n_info;

	seq_printf(seq, "%-5s %-32s %-7s %-7s %-10s %-10s\n",
			"Index", "Name", "Default", "User", "Size", "DMA Addr");
	seq_printf(seq, "%-5s %-32s %-7s %-7s 0x%-8x 0x%-8x\n",
			"N/A", "n2h_rings", "SDRAM",
			nss_meminfo_memtype_table[n2h_info->memtype],
			n2h_info->total_size, n2h_info->dma_addr);
	seq_printf(seq, "%-5s %-32s %-7s %-7s 0x%-8x 0x%-8x\n",
			"N/A", "h2n_rings", "SDRAM",
			nss_meminfo_memtype_table[h2n_info->memtype],
			h2n_info->total_size, h2n_info->dma_addr);

	r = map->requests;
	for (i = 0; i < map->num_requests; i++) {
		seq_printf(seq, "%-5d %-32s %-7s %-7s 0x%-8x 0x%-8x\n",
				i, r[i].name,
				nss_meminfo_memtype_table[r[i].memtype_default],
				nss_meminfo_memtype_table[r[i].memtype_user],
				r[i].size, r[i].addr);
	}

	seq_printf(seq, "Available IMEM: 0x%x\n", mem_ctx->imem_end - mem_ctx->imem_tail);
	seq_printf(seq, "How to configure? \n");
	seq_printf(seq, "Overwrite the /etc/modules.d/32-qca-nss-drv with following contents then reboot\n\n");
	seq_printf(seq, "qca-nss-drv meminfo_user_config=\"<core_id, name, memory_type>, ..\"\n\n");
	seq_printf(seq, "For example, <1, h2n_rings, IMEM> stands for: h2n_rings of core 1 is on IMEM\n");
	seq_printf(seq, "Note:UTCM_SHARED cannot be used for n2h_rings, h2n_rings and debug_log_boot_desc.\n");

	return 0;
}

/*
 * nss_meminfo_debugfs_file_open()
 *	function to open meminfo debugfs.
 */
static int nss_meminfo_debugfs_file_open(struct inode *inode, struct file *file)
{
	return single_open(file, nss_meminfo_config_show, inode->i_private);
}

static struct file_operations nss_meminfo_debugfs_ops = {
	.owner   = THIS_MODULE,
	.open    = nss_meminfo_debugfs_file_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

/*
 * nss_meminfo_init_debugfs()
 *	Init meminfo debugfs.
 */
static void nss_meminfo_init_debugfs(struct nss_ctx_instance *nss_ctx)
{
	int i;
	struct dentry *meminfo_main_dentry;
	struct dentry *meminfo_core_dentries[NSS_MAX_CORES];

	if (nss_meminfo_debugfs_exist)
		return;

	/*
	 * Create directory for showing meminfo configuration of each core.
	 */
	meminfo_main_dentry = debugfs_create_dir("meminfo", nss_top_main.top_dentry);
	if (unlikely(!meminfo_main_dentry)) {
		nss_warning("Failed to create qca-nss-drv/meminfo directory in debugfs\n");
		return;
	}

	for (i = 0; i < nss_top_main.num_nss; i++) {
		char file[10];
		snprintf(file, sizeof(file), "core%d", i);
		meminfo_core_dentries[i] = debugfs_create_file(file, 0400, meminfo_main_dentry,
						(void *)(nss_ptr_t)i, &nss_meminfo_debugfs_ops);
		if (unlikely(!meminfo_core_dentries[i])) {
			int j;
			for (j = 0; j < i; j++)
				debugfs_remove(meminfo_core_dentries[j]);
			debugfs_remove(meminfo_main_dentry);
			nss_warning("Failed to create qca-nss-drv/meminfo/%s file in debugfs", file);
			return;
		}
	}

	nss_meminfo_debugfs_exist = true;
	nss_info("nss meminfo user config: %s\n", nss_meminfo_user_config);
}

/*
 * nss_meminfo_init
 *	Initilization
 *
 */
bool nss_meminfo_init(struct nss_ctx_instance *nss_ctx)
{
	struct nss_meminfo_ctx *mem_ctx;
	uint32_t *meminfo_start;
	struct nss_meminfo_map *map;
	struct nss_top_instance *nss_top = &nss_top_main;

	mem_ctx = &nss_ctx->meminfo_ctx;

	/*
	 * meminfo_start is the label where the start address of meminfo map is stored.
	 */
	meminfo_start = (uint32_t *)ioremap_nocache(nss_ctx->load + NSS_MEMINFO_MAP_START_OFFSET,
							NSS_MEMINFO_RESERVE_AREA_SIZE);
	if (!meminfo_start) {
		nss_info_always("%px: cannot remap meminfo start\n", nss_ctx);
		return false;
	}

	/*
	 * Check meminfo start magic
	 */
	if ((uint16_t)meminfo_start[0] != NSS_MEMINFO_RESERVE_AREA_MAGIC) {
		nss_info_always("%px: failed to verify meminfo start magic\n", nss_ctx);
		return false;
	}

	map = &mem_ctx->meminfo_map;
	map->start = (uint32_t *)ioremap_cache(meminfo_start[1], NSS_MEMINFO_MAP_SIZE);
	if (!map->start) {
		nss_info_always("%px: failed to remap meminfo map\n", nss_ctx);
		return false;
	}

	/*
	 * Check meminfo map magic
	 */
	if ((uint16_t)map->start[0] != NSS_MEMINFO_MAP_START_MAGIC) {
		nss_info_always("%px: failed to verify meminfo map magic\n", nss_ctx);
		return false;
	}

	/*
	 * Meminfo map settings
	 */
	map->num_requests = 0;
	map->requests = (struct nss_meminfo_request *)(map->start + 1);

	/*
	 * Init IMEM
	 */
	nss_top->hal_ops->init_imem(nss_ctx);

	/*
	 * Init UTCM_SHARED if supported
	 */
	if (!nss_top->hal_ops->init_utcm_shared(nss_ctx, meminfo_start)) {
		nss_info_always("%px: failed to initialize UTCM_SHARED meminfo\n", nss_ctx);
		return false;
	}

	/*
	 * Init meminfo block lists
	 */
	if (!nss_meminfo_init_block_lists(nss_ctx)) {
		nss_info_always("%px: failed to initialize meminfo block lists\n", nss_ctx);
		return false;
	}

	/*
	 * Configure N2H/H2N rings and nss_if_mem_map
	 */
	if (!nss_meminfo_configure_n2h_h2n_rings(nss_ctx))
		return false;

	nss_meminfo_init_debugfs(nss_ctx);

	nss_info_always("%px: meminfo init succeed\n", nss_ctx);
	return true;
}
