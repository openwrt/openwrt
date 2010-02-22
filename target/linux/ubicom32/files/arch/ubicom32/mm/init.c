/*
 * arch/ubicom32/mm/init.c
 *   Ubicom32 architecture virtual memory initialization.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *  Copyright (C) 1998  D. Jeff Dionne <jeff@lineo.ca>,
 *                      Kenneth Albanowski <kjahds@kjahds.com>,
 *  Copyright (C) 2000  Lineo, Inc.  (www.lineo.com)
 *
 *  Based on:
 *
 *  linux/arch/m68k/mm/init.c
 *
 *  Copyright (C) 1995  Hamish Macdonald
 *
 *  JAN/1999 -- hacked to support ColdFire (gerg@snapgear.com)
 *  DEC/2000 -- linux 2.4 support <davidm@snapgear.com>
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/pagemap.h>
#include <linux/bootmem.h>
#include <linux/slab.h>

#include <asm/setup.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/machdep.h>
#include <asm/ocm-alloc.h>
#include <asm/processor.h>

#undef DEBUG

extern void die_if_kernel(char *,struct pt_regs *,long);
extern void free_initmem(void);

/*
 * BAD_PAGE is the page that is used for page faults when linux
 * is out-of-memory. Older versions of linux just did a
 * do_exit(), but using this instead means there is less risk
 * for a process dying in kernel mode, possibly leaving a inode
 * unused etc..
 *
 * BAD_PAGETABLE is the accompanying page-table: it is initialized
 * to point to BAD_PAGE entries.
 *
 * ZERO_PAGE is a special page that is used for zero-initialized
 * data and COW.
 */
static unsigned long empty_bad_page_table;

static unsigned long empty_bad_page;

unsigned long empty_zero_page;

void show_mem(void)
{
    unsigned long i;
    int free = 0, total = 0, reserved = 0, shared = 0;
    int cached = 0;

    printk(KERN_INFO "\nMem-info:\n");
    show_free_areas();
    i = max_mapnr;
    while (i-- > 0) {
	total++;
	if (PageReserved(mem_map+i))
	    reserved++;
	else if (PageSwapCache(mem_map+i))
	    cached++;
	else if (!page_count(mem_map+i))
	    free++;
	else
	    shared += page_count(mem_map+i) - 1;
    }
    printk(KERN_INFO "%d pages of RAM\n",total);
    printk(KERN_INFO "%d free pages\n",free);
    printk(KERN_INFO "%d reserved pages\n",reserved);
    printk(KERN_INFO "%d pages shared\n",shared);
    printk(KERN_INFO "%d pages swap cached\n",cached);
}

extern unsigned long memory_start;
extern unsigned long memory_end;
extern char __ocm_free_begin;
extern char __ocm_free_end;

/*
 * paging_init() continues the virtual memory environment setup which
 * was begun by the code in arch/head.S.
 * The parameters are pointers to where to stick the starting and ending
 * addresses of available kernel virtual memory.
 */
void __init paging_init(void)
{
	/*
	 * Make sure start_mem is page aligned, otherwise bootmem and
	 * page_alloc get different views of the world.
	 */
#ifdef DEBUG
	unsigned long start_mem = PAGE_ALIGN(memory_start);
#endif
	unsigned long end_mem   = memory_end & PAGE_MASK;

#ifdef DEBUG
	printk (KERN_DEBUG "start_mem is %#lx\nvirtual_end is %#lx\n",
		start_mem, end_mem);
#endif

	/*
	 * Initialize the bad page table and bad page to point
	 * to a couple of allocated pages.
	 */
	empty_bad_page_table = (unsigned long)alloc_bootmem_pages(PAGE_SIZE);
	empty_bad_page = (unsigned long)alloc_bootmem_pages(PAGE_SIZE);
	empty_zero_page = (unsigned long)alloc_bootmem_pages(PAGE_SIZE);
	memset((void *)empty_zero_page, 0, PAGE_SIZE);

	/*
	 * TODO: enable setting up for user memory management interface.
	 */

#ifdef DEBUG
	printk (KERN_DEBUG "before free_area_init\n");

	printk (KERN_DEBUG "free_area_init -> start_mem is %#lx\nvirtual_end is %#lx\n",
		start_mem, end_mem);
#endif

	{
		unsigned long zones_size[MAX_NR_ZONES] = {0, };
#ifdef CONFIG_ZONE_DMA
		zones_size[ZONE_DMA] = OCMSIZE >> PAGE_SHIFT;
#endif
		zones_size[ZONE_NORMAL] = (end_mem - PAGE_OFFSET) >> PAGE_SHIFT;
#ifdef CONFIG_HIGHMEM
		zones_size[ZONE_HIGHMEM] = 0;
#endif
		free_area_init(zones_size);
	}
}

void __init mem_init(void)
{
	int codek = 0, datak = 0, initk = 0;
	unsigned long tmp, ram_start, ram_end, len;
	extern char _etext, _stext, _sdata, _ebss, __init_begin, __init_end;

	unsigned long start_mem = memory_start; /* DAVIDM - these must start at end of kernel */
	unsigned long end_mem   = memory_end; /* DAVIDM - this must not include kernel stack at top */
	processor_dram(&ram_start, &ram_end);
	len = (ram_end - ram_start) + OCMSIZE;
#ifdef DEBUG
	printk(KERN_DEBUG "Mem_init: start=%lx, end=%lx\n", start_mem, end_mem);
#endif

	end_mem &= PAGE_MASK;
	high_memory = (void *) end_mem;

	start_mem = PAGE_ALIGN(start_mem);
	max_mapnr = num_physpages = (((unsigned long) high_memory) - PAGE_OFFSET) >> PAGE_SHIFT;

	/* this will put all memory onto the freelists */
#ifdef CONFIG_ZONE_DMA
	{
		unsigned long ocm_free_begin = (unsigned long)&__ocm_free_begin;
		unsigned long ocm_free_end = (unsigned long)&__ocm_free_end;
		unsigned long zone_dma_begin = (ocm_free_begin + PAGE_SIZE - 1) & PAGE_MASK;
		unsigned long zone_dma_end = ocm_free_end & PAGE_MASK;
		if (zone_dma_end > zone_dma_begin)
			free_bootmem(zone_dma_begin, zone_dma_end-zone_dma_begin);
	}
#endif
	totalram_pages = free_all_bootmem();

	codek = (&_etext - &_stext) >> 10;
	datak = (&_ebss - &_sdata) >> 10;
	initk = (&__init_begin - &__init_end) >> 10;

	tmp = nr_free_pages() << PAGE_SHIFT;
	printk(KERN_INFO "Memory available: %luk/%luk RAM, (%dk kernel code, %dk data)\n",
	       tmp >> 10,
	       len >> 10,
	       codek,
	       datak
	       );

}

#ifdef CONFIG_BLK_DEV_INITRD
void free_initrd_mem(unsigned long start, unsigned long end)
{
	int pages = 0;
	for (; start < end; start += PAGE_SIZE) {
		ClearPageReserved(virt_to_page(start));
		init_page_count(virt_to_page(start));
		free_page(start);
		totalram_pages++;
		pages++;
	}
	printk (KERN_NOTICE "Freeing initrd memory: %dk freed\n", pages);
}
#endif

void
free_initmem()
{
#ifdef CONFIG_RAMKERNEL
	unsigned long addr;
	extern char __init_begin, __init_end;
	/*
	 * The following code should be cool even if these sections
	 * are not page aligned.
	 */
	addr = PAGE_ALIGN((unsigned long)(&__init_begin));
	/* next to check that the page we free is not a partial page */
	for (; addr + PAGE_SIZE < (unsigned long)(&__init_end); addr +=PAGE_SIZE) {
		ClearPageReserved(virt_to_page(addr));
		init_page_count(virt_to_page(addr));
		free_page(addr);
		totalram_pages++;
	}
	printk(KERN_NOTICE "Freeing unused kernel memory: %ldk freed (0x%x - 0x%x)\n",
			(addr - PAGE_ALIGN((long) &__init_begin)) >> 10,
			(int)(PAGE_ALIGN((unsigned long)(&__init_begin))),
			(int)(addr - PAGE_SIZE));
#endif
}
