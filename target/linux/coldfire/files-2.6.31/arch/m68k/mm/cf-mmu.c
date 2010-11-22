/*
 * linux/arch/m68k/mm/cf-mmu.c
 *
 * Based upon linux/arch/m68k/mm/sun3mmu.c
 * Based upon linux/arch/ppc/mm/mmu_context.c
 *
 * Implementations of mm routines specific to the Coldfire MMU.
 *
 * Copyright (c) 2008 Freescale Semiconductor, Inc.
 * Copyright Freescale Semiconductor, Inc. 2008-2009
 *   Jason Jin Jason.Jin@freescale.com
 *   Shrek Wu B16972@freescale.com
 */

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/init.h>
#ifdef CONFIG_BLK_DEV_RAM
#include <linux/blkdev.h>
#endif
#include <linux/bootmem.h>

#include <asm/setup.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/machdep.h>
#include <asm/io.h>
#include <asm/mmu_context.h>
#include <asm/cf_pgalloc.h>

#include <asm/coldfire.h>
#include <asm/tlbflush.h>

#define KMAPAREA(x)	((x >= VMALLOC_START) && (x < KMAP_END))

#undef DEBUG

#ifdef CONFIG_VDSO
unsigned long next_mmu_context;
#else
mm_context_t next_mmu_context;
#endif

unsigned long context_map[LAST_CONTEXT / BITS_PER_LONG + 1];

atomic_t nr_free_contexts;
struct mm_struct *context_mm[LAST_CONTEXT+1];
void steal_context(void);
#ifdef CONFIG_M5445X
void m68k_setup_node(int);
#endif
const char bad_pmd_string[] = "Bad pmd in pte_alloc: %08lx\n";

extern unsigned long empty_bad_page_table;
extern unsigned long empty_bad_page;
extern unsigned long num_pages;
#ifdef CONFIG_M5445X
extern unsigned long availmem;
#endif
extern char __init_begin, __init_end;

/*
 * Free memory used for system initialization.
 */
void free_initmem(void)
{
#if 0
	unsigned long addr;
	unsigned long start = (unsigned long)&__init_begin;
	unsigned long end = (unsigned long)&__init_end;

	printk(KERN_INFO "free_initmem: __init_begin = 0x%lx  __init_end = 0x%lx\n", start, end);

	addr = (unsigned long)&__init_begin;
	for (; addr < (unsigned long)&__init_end; addr += PAGE_SIZE) {
		/* not currently used */
		virt_to_page(addr)->flags &= ~(1 << PG_reserved);
		init_page_count(virt_to_page(addr));
		free_page(addr);
		totalram_pages++;
	}
#endif
}

/*
 * Initialize the paging system.
 */
void __init paging_init(void)
{
	pgd_t * pg_dir;
	pte_t * pg_table;
	int i;
	unsigned long address;
	unsigned long next_pgtable;
	unsigned long zones_size[MAX_NR_ZONES];
	unsigned long size;
	enum zone_type zone;

	/* allocate zero page */
	empty_zero_page = (void *)alloc_bootmem_pages(PAGE_SIZE);
	memset((void *)empty_zero_page, 0, PAGE_SIZE);

	/* zero kernel page directory */
	pg_dir = swapper_pg_dir;
	memset(swapper_pg_dir, 0, sizeof(swapper_pg_dir));
	/*
	 * setup page tables for PHYSRAM
	 */

	/* starting loc in page directory */
	pg_dir += PAGE_OFFSET >> PGDIR_SHIFT; 

	/* allocate page tables */
	size = num_pages * sizeof(pte_t);  
	size = (size + PAGE_SIZE) & ~(PAGE_SIZE-1);
	next_pgtable = (unsigned long)alloc_bootmem_pages(size);
	address = PAGE_OFFSET;
	while (address < (unsigned long)high_memory) {
		/* setup page table in page directory */
    		pg_table = (pte_t *)next_pgtable; 
	    	next_pgtable += PTRS_PER_PTE * sizeof(pte_t);
		pgd_val(*pg_dir) = (unsigned long)pg_table;
		pg_dir++;

		/* create PTEs in page table */
		for (i=0; i<PTRS_PER_PTE; ++i, ++pg_table) {
			pte_t pte = pfn_pte(virt_to_pfn(address), PAGE_INIT);
			if (address >= (unsigned long)high_memory)
				pte_val (pte) = 0;

			set_pte(pg_table, pte);
			address += PAGE_SIZE;
		}
	}

	/*
	 * setup page tables for DMA area
	 */

	/* starting loc in page directory */
	pg_dir = swapper_pg_dir;
	pg_dir += CONFIG_DMA_BASE >> PGDIR_SHIFT; 

	/* allocate page tables */
	size = (CONFIG_DMA_SIZE >> PAGE_SHIFT) * sizeof(pte_t);  
	size = (size + PAGE_SIZE) & ~(PAGE_SIZE-1);
	next_pgtable = (unsigned long)alloc_bootmem_pages(size);
	address = CONFIG_DMA_BASE;
	while (address < (CONFIG_DMA_BASE + CONFIG_DMA_SIZE)) {
		/* setup page table in page directory */
    		pg_table = (pte_t *)next_pgtable; 
	    	next_pgtable += PTRS_PER_PTE * sizeof(pte_t);
		pgd_val(*pg_dir) = (unsigned long)pg_table;
		pg_dir++;

		/* create PTEs in page table */
		for (i=0; i<PTRS_PER_PTE; ++i, ++pg_table) {
			pte_t pte = pfn_pte(virt_to_pfn(address), PAGE_INIT);
			if (address >= (CONFIG_DMA_BASE + CONFIG_DMA_SIZE))
				pte_val (pte) = 0;

			set_pte(pg_table, pte);
			address += PAGE_SIZE;
		}
	}

	/*
	 * setup zones
	 */

	current->mm = NULL;

	/* clear zones */
	for (zone = 0; zone < MAX_NR_ZONES; zone++)
		zones_size[zone] = 0x0;

	zones_size[ZONE_DMA] = CONFIG_DMA_SIZE >> PAGE_SHIFT;
	zones_size[ZONE_NORMAL] = (((unsigned long)high_memory -
				    PAGE_OFFSET) >> PAGE_SHIFT) -
				   zones_size[ZONE_DMA];

	free_area_init(zones_size);
}
/*
 * Handle a missed TLB
 */
int cf_tlb_miss(struct pt_regs *regs, int write, int dtlb, int extension_word)
{
        struct mm_struct *mm;
        pgd_t *pgd;
        pmd_t *pmd;
        pte_t *pte;
        unsigned long mmuar;
        int asid;
	int flags;

	local_save_flags(flags);
	local_irq_disable();

	mmuar = ( dtlb ) ? regs->mmuar
			 : regs->pc + (extension_word * sizeof(long));

        mm = (!user_mode(regs) && KMAPAREA(mmuar)) ? &init_mm : current->mm;

        if (!mm) {
	    local_irq_restore(flags);
	    return (-1);
	}

        pgd = pgd_offset(mm, mmuar);
        if (pgd_none(*pgd))  {
	    local_irq_restore(flags);
	    return (-1);
	}
	    
    	pmd = pmd_offset(pgd, mmuar);
    	if (pmd_none(*pmd)) {
	    local_irq_restore(flags);
	    return (-1);
	}	
    
	pte = (KMAPAREA(mmuar)) ? pte_offset_kernel(pmd, mmuar)
	                       : pte_offset_map(pmd, mmuar);
    	if (pte_none(*pte) || !pte_present(*pte)) {
	    local_irq_restore(flags);
	    return (-1);		
	}

	if (write) {
            if (!pte_write(*pte)) {
		local_irq_restore(flags);
	    	return (-1);
	    }
    	    set_pte(pte, pte_mkdirty(*pte));
	}
	
        set_pte(pte, pte_mkyoung(*pte));
        asid = cpu_context(mm) & 0xff;
        if (!pte_dirty(*pte) && !KMAPAREA(mmuar))
    	    set_pte(pte, pte_wrprotect(*pte));

        *MMUTR = (mmuar & PAGE_MASK) | (asid << CF_ASID_MMU_SHIFT)
               | (((int)(pte->pte) & (int)CF_PAGE_MMUTR_MASK ) >> CF_PAGE_MMUTR_SHIFT)
               | MMUTR_V;

        *MMUDR = (pte_val(*pte) & PAGE_MASK) 
	       | ((pte->pte) & CF_PAGE_MMUDR_MASK)
               | MMUDR_SZ8K | MMUDR_X;
	    
	if ( dtlb )
    	    *MMUOR = MMUOR_ACC | MMUOR_UAA;
	else
	    *MMUOR = MMUOR_ITLB | MMUOR_ACC | MMUOR_UAA;

	asm("nop");

#ifdef DEBUG
	printk("cf_tlb_miss: va=%lx, pa=%lx\n", (mmuar & PAGE_MASK), 
		  (pte_val(*pte)  & PAGE_MASK));
#endif
	local_irq_restore(flags);
        return (0);
}


/*
 * Context Management
 *
 * Based on arch/ppc/mmu_context.c
 */

/*
 * Initialize the context management system.
 */
void __init mmu_context_init(void)
{
	/*
	 * Some processors have too few contexts to reserve one for
	 * init_mm, and require using context 0 for a normal task.
	 * Other processors reserve the use of context zero for the kernel.
	 * This code assumes FIRST_CONTEXT < 32.
	 */
	context_map[0] = (1 << FIRST_CONTEXT) - 1;
	next_mmu_context = FIRST_CONTEXT;
	atomic_set(&nr_free_contexts, LAST_CONTEXT - FIRST_CONTEXT + 1);
}

/*
 * Steal a context from a task that has one at the moment.
 * This is only used on 8xx and 4xx and we presently assume that
 * they don't do SMP.  If they do then thicfpgalloc.hs will have to check
 * whether the MM we steal is in use.
 * We also assume that this is only used on systems that don't
 * use an MMU hash table - this is true for 8xx and 4xx.
 * This isn't an LRU system, it just frees up each context in
 * turn (sort-of pseudo-random replacement :).  This would be the
 * place to implement an LRU scheme if anyone was motivated to do it.
 *  -- paulus
 */
void steal_context(void)
{
	struct mm_struct *mm;
	/* free up context `next_mmu_context' */
	/* if we shouldn't free context 0, don't... */
	if (next_mmu_context < FIRST_CONTEXT)
		next_mmu_context = FIRST_CONTEXT;
	mm = context_mm[next_mmu_context];
	flush_tlb_mm(mm);
	destroy_context(mm);
}
