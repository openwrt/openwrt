/*
 * arch/ubicom32/include/asm/pgtable.h
 *   Ubicom32 pseudo page table definitions and operations.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 2004   Microtronix Datacom Ltd
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
 *   and various works, Alpha, ix86, M68K, Sparc, ...et al
 */
#ifndef _ASM_UBICOM32_PGTABLE_H
#define _ASM_UBICOM32_PGTABLE_H

#include <asm-generic/4level-fixup.h>

//vic - this bit copied from m68knommu version
#include <asm/setup.h>
#include <asm/io.h>
#include <linux/sched.h>

typedef pte_t *pte_addr_t;

#define pgd_present(pgd)     	(1)       /* pages are always present on NO_MM */
#define pgd_none(pgd)		(0)
#define pgd_bad(pgd)		(0)
#define pgd_clear(pgdp)
#define kern_addr_valid(addr) 	(1)
#define	pmd_offset(a, b)	((void *)0)

#define PAGE_NONE		__pgprot(0)    /* these mean nothing to NO_MM */
#define PAGE_SHARED		__pgprot(0)    /* these mean nothing to NO_MM */
#define PAGE_COPY		__pgprot(0)    /* these mean nothing to NO_MM */
#define PAGE_READONLY		__pgprot(0)    /* these mean nothing to NO_MM */
#define PAGE_KERNEL		__pgprot(0)    /* these mean nothing to NO_MM */
//vic - this bit copied from m68knommu version

extern void paging_init(void);
#define swapper_pg_dir ((pgd_t *) 0)

#define __swp_type(x)		(0)
#define __swp_offset(x)		(0)
#define __swp_entry(typ,off)	((swp_entry_t) { ((typ) | ((off) << 7)) })
#define __pte_to_swp_entry(pte)	((swp_entry_t) { pte_val(pte) })
#define __swp_entry_to_pte(x)	((pte_t) { (x).val })

/*
 * pgprot_noncached() is only for infiniband pci support, and a real
 * implementation for RAM would be more complicated.
 */
#define pgprot_noncached(prot)	(prot)

static inline int pte_file(pte_t pte) { return 0; }

/*
 * ZERO_PAGE is a global shared page that is always zero: used
 * for zero-mapped memory areas etc..
 */
#define ZERO_PAGE(vaddr)	(virt_to_page(0))

extern unsigned int kobjsize(const void *objp);
extern int is_in_rom(unsigned long);

/*
 * No page table caches to initialise
 */
#define pgtable_cache_init()   do { } while (0)

#define io_remap_pfn_range(vma, vaddr, pfn, size, prot)		\
		remap_pfn_range(vma, vaddr, pfn, size, prot)

extern inline void flush_cache_mm(struct mm_struct *mm)
{
}

extern inline void flush_cache_range(struct mm_struct *mm,
				     unsigned long start,
				     unsigned long end)
{
}

/* Push the page at kernel virtual address and clear the icache */
extern inline void flush_page_to_ram (unsigned long address)
{
}

/* Push n pages at kernel virtual address and clear the icache */
extern inline void flush_pages_to_ram (unsigned long address, int n)
{
}

/*
 * All 32bit addresses are effectively valid for vmalloc...
 * Sort of meaningless for non-VM targets.
 */
#define	VMALLOC_START	0
#define	VMALLOC_END	0xffffffff

#define arch_enter_lazy_mmu_mode()	do {} while (0)
#define arch_leave_lazy_mmu_mode()	do {} while (0)
#define arch_flush_lazy_mmu_mode()	do {} while (0)
#define arch_enter_lazy_cpu_mode()	do {} while (0)
#define arch_leave_lazy_cpu_mode()	do {} while (0)
#define arch_flush_lazy_cpu_mode()	do {} while (0)

#endif /* _ASM_UBICOM32_PGTABLE_H */
