/*
 * $Id$
 * 
 * Copyright (C) 2007 OpenWrt.org
 * 
 * Based on arch/mips/mm/init.c
 * Copyright (C) 1994 - 2000 Ralf Baechle
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 * Kevin D. Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000 MIPS Technologies, Inc.  All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <linux/bootmem.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pfn.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/swap.h>

#include <asm/bootinfo.h>
#include <asm/page.h>
#include <asm/sections.h>

#include <asm/mips-boards/prom.h>

#warning FIXME: use sdram control regs and/or autodetection
static int __init memsize(void)
{
	char *memsize_str;
	unsigned int result;
	char cmdline[CL_SIZE], *ptr;

	/* Check the command line first for a memsize directive */
	strcpy(cmdline, arcs_cmdline);
	ptr = strstr(cmdline, "memsize=");
	if (ptr && (ptr != cmdline) && (*(ptr - 1) != ' '))
		ptr = strstr(ptr, " memsize=");

	if (ptr) {
		result = memparse(ptr + 8, &ptr);
	} else {
		/* otherwise look in the environment */
		memsize_str = prom_getenv("memsize");
		if (!memsize_str) {
			prom_printf("memsize not set in boot prom, set to default (8Mb)\n");
			result = 0x00800000;
		} else {
			result = simple_strtol(memsize_str, NULL, 0);
		}
	}

	return result;
}

#ifdef CONFIG_NEED_MULTIPLE_NODES
static bootmem_data_t node_bootmem_data;
pg_data_t __node_data[1] = {
	{ 
		.bdata = &node_bootmem_data 
	},
};
EXPORT_SYMBOL(__node_data);

unsigned long max_mapnr;
struct page *mem_map;
EXPORT_SYMBOL(max_mapnr);
EXPORT_SYMBOL(mem_map);

static unsigned long setup_zero_pages(void)
{
	unsigned int order = 3;
	unsigned long size;
	struct page *page;

	empty_zero_page = __get_free_pages(GFP_KERNEL | __GFP_ZERO, order);
	if (!empty_zero_page)
		panic("Oh boy, that early out of memory?");

	page = virt_to_page(empty_zero_page);
	split_page(page, order);
	while (page < virt_to_page(empty_zero_page + (PAGE_SIZE << order))) {
		SetPageReserved(page);
		page++;
	}

	size = PAGE_SIZE << order;
	zero_page_mask = (size - 1) & PAGE_MASK;

	return 1UL << order;
}

extern void pagetable_init(void);

void __init paging_init(void)
{
	unsigned long zones_size[MAX_NR_ZONES] = { 0, };

	pagetable_init();

	zones_size[ZONE_DMA] = max_low_pfn - min_low_pfn;

	free_area_init_node(0, NODE_DATA(0), zones_size, ARCH_PFN_OFFSET, NULL);
}

static struct kcore_list kcore_mem, kcore_vmalloc;

void __init mem_init(void)
{
	unsigned long codesize, reservedpages, datasize, initsize;
	unsigned long tmp, ram;
	unsigned long kernel_start, kernel_end;

	kernel_start = PFN_DOWN(CPHYSADDR((unsigned long)&_text));
	kernel_end = PFN_UP(CPHYSADDR((unsigned long)&_end));
	for (tmp = min_low_pfn + 1; tmp < kernel_start; tmp++) {
		ClearPageReserved(pfn_to_page(tmp));
		init_page_count(pfn_to_page(tmp));
		free_page((unsigned long)__va(tmp << PAGE_SHIFT));
	}

	totalram_pages += free_all_bootmem();
	totalram_pages -= setup_zero_pages();	/* Setup zeroed pages.  */

	reservedpages = ram = 0;
	for (tmp = min_low_pfn; tmp <= max_low_pfn; tmp++) {
		ram++;
		if (PageReserved(pfn_to_page(tmp)))
			if ((tmp < kernel_start) || (tmp > kernel_end)) 
				reservedpages++;
	}
	num_physpages = ram;

	codesize =  (unsigned long) &_etext - (unsigned long) &_text;
	datasize =  (unsigned long) &_edata - (unsigned long) &_etext;
	initsize =  (unsigned long) &__init_end - (unsigned long) &__init_begin;

	kclist_add(&kcore_mem, __va(min_low_pfn), 
		   (max_low_pfn - min_low_pfn) << PAGE_SHIFT);
	kclist_add(&kcore_vmalloc, (void *)VMALLOC_START,
		   VMALLOC_END - VMALLOC_START);

	printk(KERN_INFO "Memory: %luk/%luk available (%ldk kernel code, "
	       "%ldk reserved, %ldk data, %ldk init)\n",
	       (unsigned long) nr_free_pages() << (PAGE_SHIFT-10),
	       ram << (PAGE_SHIFT-10),
	       codesize >> 10,
	       reservedpages << (PAGE_SHIFT-10),
	       datasize >> 10,
	       initsize >> 10);
}
#endif

void __init prom_meminit(void)
{
#ifdef CONFIG_NEED_MULTIPLE_NODES
	unsigned long kernel_start, kernel_end;
	unsigned long pages, free_pages;
	unsigned long bootmap_size;
#endif

	pages = memsize() >> PAGE_SHIFT;
	add_memory_region(ARCH_PFN_OFFSET << PAGE_SHIFT, pages <<
			  PAGE_SHIFT, BOOT_MEM_RAM);

#ifdef CONFIG_NEED_MULTIPLE_NODES
	kernel_start = PFN_DOWN(CPHYSADDR((unsigned long)&_text));
	kernel_end = PFN_UP(CPHYSADDR((unsigned long)&_end));
	min_low_pfn = ARCH_PFN_OFFSET;
	max_low_pfn = ARCH_PFN_OFFSET + pages;
	max_mapnr = max_low_pfn;
	free_pages = pages - (kernel_end - min_low_pfn);
	bootmap_size = init_bootmem_node(NODE_DATA(0), kernel_end,
					 ARCH_PFN_OFFSET, max_low_pfn);

	free_bootmem(PFN_PHYS(kernel_end), free_pages << PAGE_SHIFT);
	memory_present(0, min_low_pfn, max_low_pfn);
	reserve_bootmem(PFN_PHYS(kernel_end), bootmap_size);
	mem_map = NODE_DATA(0)->node_mem_map;
#endif
}

unsigned long __init prom_free_prom_memory(void)
{
/*	return freed;
*/
	return 0;
}
