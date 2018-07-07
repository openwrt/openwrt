/*
 *  Lexra specific mmu/cache code.
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/sizes.h>

#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/mmu_context.h>
#include <asm/isadep.h>
#include <asm/io.h>
#include <asm/cpu.h>
#include <asm/cacheops.h>
#include <asm/lxregs.h>
#include <asm/barrier.h>
#include <asm/c-lexra.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>

#define LEXRA_CCTL_BARRIER \
	do { \
		__asm__ __volatile__ ( "" : : : "memory"); \
	} while (0)

unsigned long lexra_icache_size, lexra_dcache_size;	/* Size in bytes */
unsigned long lexra_icache_lsize, lexra_dcache_lsize;	/* Size in bytes */

int lexra_has_dcacheop;
int lexra_has_wb_dcache;

int lexra_imem0_size = SZ_4K;
int lexra_imem1_size = SZ_4K;
int lexra_has_dual_cmem = 0;

extern char __iram;
extern char __dram_start;
extern char __dram_end;

static void lexra_cmem_init(void)
{
	u32 iram_base = CPHYSADDR(((u32) &__iram) & ~(SZ_16K - 1));
	u32 dram_base = CPHYSADDR(((u32) &__dram_start) & ~(SZ_8K - 1));

	/* XXX: do not use it at present */
	return;

	/* enable co-processor 3 */
	write_c0_status(read_c0_status() | ST0_CU3);

	/* I-MEM off */
	write_c0_cctl(0);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(CCTL_IMEMOFF);

	/* I-Cache/D-Cache invalidate */
	if (lexra_has_wb_dcache) {
		write_c0_cctl(0);
		LEXRA_CCTL_BARRIER;
		write_c0_cctl(CCTL_IInval | CCTL_DWBInval);
	} else {
		write_c0_cctl(0);
		LEXRA_CCTL_BARRIER;
		write_c0_cctl(CCTL_IInval | CCTL_DInval);
	}

	if (!lexra_has_dual_cmem) {
		/* set I-MEM base and size */
		write_c3_imembase(iram_base);
#if 0
		if (soc_is_rtl819xd())
			write_c3_imemtop(iram_base + SZ_8K - 1);
		else
#endif
			write_c3_imemtop(iram_base + SZ_16K - 1);

		/* I-MEM refill */
		write_c0_cctl(0);
		LEXRA_CCTL_BARRIER;
		write_c0_cctl(CCTL_IMEMFILL);

		if (&__dram_start < &__dram_end) {
			/* set D-MEM base and size */
			write_c3_dmembase(dram_base);
			write_c3_dmemtop(dram_base + SZ_8K - 1);

			/* D-MEM on */
			write_c0_cctl(0);
			LEXRA_CCTL_BARRIER;
			write_c0_cctl(CCTL_DMEMON);
		}
	} else {
		/* set I-MEM0 base and size */
		write_c3_imembase(iram_base);
		write_c3_imemtop(iram_base + lexra_imem0_size - 1);

		/* I-MEM0 refill */
		write_c0_cctl(0);
		LEXRA_CCTL_BARRIER;
		write_c0_cctl(CCTL_IMEMFILL);

		/* set I-MEM1 base and size */
		write_c3_imem1base(iram_base + lexra_imem0_size);
		write_c3_imem1top(iram_base + lexra_imem0_size + lexra_imem1_size - 1);

		/* I-MEM1 refill */
		write_c0_cctl1(0);
		LEXRA_CCTL_BARRIER;
		write_c0_cctl1(CCTL_IMEMFILL);

		if (&__dram_start < &__dram_end) {
			/* set D-MEM0 base and size */
			write_c3_dmembase(dram_base);
			write_c3_dmemtop(dram_base + SZ_4K - 1);

			/* D-MEM0 on */
			write_c0_cctl(0);
			LEXRA_CCTL_BARRIER;
			write_c0_cctl(CCTL_DMEMON);

			/* set D-MEM1 base and size */
			write_c3_dmem1base(dram_base + SZ_4K);
			write_c3_dmem1top(dram_base + SZ_8K - 1);

			/* D-MEM1 on */
			write_c0_cctl1(0);
			LEXRA_CCTL_BARRIER;
			write_c0_cctl1(CCTL_DMEMON);
		}
	}

	/* flush */
	write_c0_cctl(0);
}

static void lexra_wb_inv_dcache_all(void)
{
	write_c0_cctl(0);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(CCTL_DWBInval);
}

static void lexra_inv_dcache_all(void)
{
	write_c0_cctl(0);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(CCTL_DInval);
}

static void lexra_wb_dcache_all(void)
{
	write_c0_cctl(0);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(CCTL_DWB);
}

static void lexra_inv_icache_all(void)
{
	write_c0_cctl(0);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(CCTL_IInval);
}

static void lexra_wb_inv_dcache_range(unsigned long start, unsigned long end)
{
	unsigned long size, p, flags;

	start &= ~(lexra_dcache_lsize - 1);
	size = end - start;

	if (!lexra_has_dcacheop || (size >= lexra_dcache_size * 2)) {
		lexra_wb_inv_dcache_all();
		return;
	}

	flags = read_c0_status();

    /* disable interrupt */
    write_c0_status(flags & ~ST0_IEC);

	for (p = start; p < end; p += lexra_dcache_lsize) {
		__asm__ __volatile__ (
			".set\tpush\n"
			".set\tmips3\n"
			"cache\t%0, 0(%1)\n"
			".set\tpop"
			: : "i" (Hit_Writeback_Inv_D), "r" (p)
		);
	}

    /* restore interrupt */
    write_c0_status(flags);
}

static void lexra_inv_dcache_range(unsigned long start, unsigned long end)
{
	unsigned long size, p, flags;

	start &= ~(lexra_dcache_lsize - 1);
	size = end - start;

	if (!lexra_has_dcacheop || (size >= lexra_dcache_size * 2)) {
		lexra_inv_dcache_all();
		return;
	}

	flags = read_c0_status();

    /* disable interrupt */
    write_c0_status(flags & ~ST0_IEC);

	for (p = start; p < end; p += lexra_dcache_lsize) {
		__asm__ __volatile__ (
			".set\tpush\n"
			".set\tmips3\n"
			"cache\t%0, 0(%1)\n"
			".set\tpop"
			: : "i" (Hit_Invalidate_D), "r" (p)
		);
	}

    /* restore interrupt */
    write_c0_status(flags);
}

static void lexra_wb_dcache_range(unsigned long start, unsigned long end)
{
	unsigned long size, p, flags;

	start &= ~(lexra_dcache_lsize - 1);
	size = end - start;

	if (!lexra_has_dcacheop || (size >= lexra_dcache_size * 2)) {
		lexra_wb_dcache_all();
		return;
	}

	flags = read_c0_status();

    /* disable interrupt */
    write_c0_status(flags & ~ST0_IEC);

	for (p = start; p < end; p += lexra_dcache_lsize) {
		__asm__ __volatile__ (
			".set\tpush\n"
			".set\tmips3\n"
			"cache\t%0, 0(%1)\n"
			".set\tpop"
			: : "i" (Hit_Writeback_D), "r" (p)
		);
	}

    /* restore interrupt */
    write_c0_status(flags);
}

static void lexra_flush_dcache_range(unsigned long start, unsigned long end)
{
	if (lexra_has_wb_dcache)
		lexra_wb_inv_dcache_range(start, end);
	else
		lexra_inv_dcache_range(start, end);
}

static void lexra_wback_dcache_range(unsigned long start, unsigned long end)
{
	if (lexra_has_wb_dcache)
		lexra_wb_dcache_range(start, end);
}

static void lexra_flush_dcache_all(void)
{
	if (lexra_has_wb_dcache)
		lexra_wb_inv_dcache_all();
	else
		lexra_inv_dcache_all();
}

static void lexra_flush_icache_range(unsigned long start, unsigned long end)
{
	lexra_wback_dcache_range(start, end);
    lexra_inv_icache_all();
}


static inline void lexra_flush_cache_all(void)
{
}

static inline void lexra___flush_cache_all(void)
{
	lexra_flush_dcache_all();
	lexra_inv_icache_all();
}

static void lexra_flush_cache_mm(struct mm_struct *mm)
{
}

static void lexra_flush_cache_range(struct vm_area_struct *vma,
				  unsigned long start, unsigned long end)
{
}

static void lexra_flush_cache_page(struct vm_area_struct *vma,
				 unsigned long addr, unsigned long pfn)
{
	unsigned long kaddr = KSEG0ADDR(pfn << PAGE_SHIFT);
	int exec = vma->vm_flags & VM_EXEC;
	struct mm_struct *mm = vma->vm_mm;
	pgd_t *pgdp;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;

	pr_debug("cpage[%08lx,%08lx]\n",
		 cpu_context(smp_processor_id(), mm), addr);

	/* No ASID => no such page in the cache.  */
	if (cpu_context(smp_processor_id(), mm) == 0)
		return;

	pgdp = pgd_offset(mm, addr);
	pudp = pud_offset(pgdp, addr);
	pmdp = pmd_offset(pudp, addr);
	ptep = pte_offset(pmdp, addr);

	/* Invalid => no such page in the cache.  */
	if (!(pte_val(*ptep) & _PAGE_PRESENT))
		return;

	lexra_flush_dcache_range(kaddr, kaddr + PAGE_SIZE);
	if (exec)
		lexra_inv_icache_all();
}

static void local_lexra_flush_data_cache_page(void *addr)
{
}

static void lexra_flush_data_cache_page(unsigned long addr)
{
}

static void lexra_flush_cache_sigtramp(unsigned long addr)
{
	unsigned long flags;

	pr_debug("csigtramp[%08lx]\n", addr);

	flags = read_c0_status();

	write_c0_status(flags & ~ST0_IEC);

	/* Fill the TLB to avoid an exception with caches isolated. */
	lexra_flush_icache_range(addr, lexra_icache_lsize);

	write_c0_status(flags);
}

static void lexra_flush_kernel_vmap_range(unsigned long vaddr, int size)
{
	BUG();
}

static void lexra_dma_cache_wback_inv(unsigned long start, unsigned long size)
{
	/* Catch bad driver code */
	BUG_ON(size == 0);

	iob();
	lexra_flush_dcache_range(start, start + size);
}

static void lexra_probe_cache(void)
{
	/* Set default values */
	lexra_dcache_lsize = 16;
	lexra_icache_lsize = 16;
	lexra_dcache_size = SZ_8K;
	lexra_icache_size = SZ_16K;
	lexra_has_dcacheop = 0;
	lexra_has_wb_dcache = 0;

	switch (current_cpu_type()) {
	case CPU_LX4180:
	case CPU_LX4189:
	case CPU_LX4280:
	case CPU_LX5180:
	case CPU_LX5280:
	case CPU_LX8000:
		break;
	case CPU_LX4380:
	case CPU_LX8380:
		lexra_has_dcacheop = 1;
		lexra_has_wb_dcache = 1;
		break;
	default:
		BUG();
	}

	/* Override by specific platform */
	plat_lexra_cache_init();

	cpu_data[0].dcache.linesz = lexra_dcache_lsize;
	cpu_data[0].icache.linesz = lexra_icache_lsize;
}

void r3k_cache_init(void)
{
	extern void build_clear_page(void);
	extern void build_copy_page(void);

	lexra_probe_cache();

	flush_cache_all = lexra_flush_cache_all;
	__flush_cache_all = lexra___flush_cache_all;
	flush_cache_mm = lexra_flush_cache_mm;
	flush_cache_range = lexra_flush_cache_range;
	flush_cache_page = lexra_flush_cache_page;
	flush_icache_range = lexra_flush_icache_range;
	local_flush_icache_range = lexra_flush_icache_range;

	__flush_kernel_vmap_range = lexra_flush_kernel_vmap_range;

	flush_cache_sigtramp = lexra_flush_cache_sigtramp;
	local_flush_data_cache_page = local_lexra_flush_data_cache_page;
	flush_data_cache_page = lexra_flush_data_cache_page;

	_dma_cache_wback_inv = lexra_dma_cache_wback_inv;
	_dma_cache_wback = lexra_dma_cache_wback_inv;
	_dma_cache_inv = lexra_dma_cache_wback_inv;

	printk("Primary instruction cache %ldkB, linesize %ld bytes.\n",
		lexra_icache_size >> 10, lexra_icache_lsize);
	printk("Primary write-%s data cache %ldkB, linesize %ld bytes.\n",
		lexra_has_wb_dcache ? "back" : "through",
		lexra_dcache_size >> 10, lexra_dcache_lsize);

	lexra_cmem_init();

	build_clear_page();
	build_copy_page();
}
