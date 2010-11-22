/*
 * linux/include/asm-m68k/cf_page.h
 *
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * Based on linux/include/asm-m68k/page.h
 *
 * 10/09/08 JKM: split Coldfire pieces into separate file
 */
#ifndef __CF_PAGE__
#define __CF_PAGE__

#include <linux/const.h>
#include <asm/setup.h>
#include <asm/page_offset.h>

/* Virtual base page location */
#define PAGE_OFFSET	(PAGE_OFFSET_RAW)

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT	(13)	/* 8K pages */
#define PAGE_SIZE	(_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define THREAD_SIZE 	PAGE_SIZE

#ifndef __ASSEMBLY__
#include <linux/compiler.h>
#include <asm/module.h>

#define get_user_page(vaddr)		__get_free_page(GFP_KERNEL)
#define free_user_page(page, addr)	free_page(addr)

#define clear_page(page)	memset((page), 0, PAGE_SIZE)
#define copy_page(to,from)	memcpy((to), (from), PAGE_SIZE)

#define clear_user_page(addr, vaddr, page)	\
	do {	clear_page(addr);		\
		flush_dcache_page(page);	\
	} while (0)

#define copy_user_page(to, from, vaddr, page)	\
	do {	copy_page(to, from);		\
		flush_dcache_page(page);	\
	} while (0)

/*
 * These are used to make use of C type-checking..
 */
typedef struct { unsigned long pte; } pte_t;
typedef struct { unsigned long pmd[16]; } pmd_t;
typedef struct { unsigned long pgd; } pgd_t;
typedef struct { unsigned long pgprot; } pgprot_t;
typedef struct page *pgtable_t;

#define pte_val(x)	((x).pte)
#define pmd_val(x)	((&x)->pmd[0])
#define pgd_val(x)	((x).pgd)
#define pgprot_val(x)	((x).pgprot)

#define __pte(x)	((pte_t) { (x) } )
#define __pmd(x)	((pmd_t) { (x) } )
#define __pgd(x)	((pgd_t) { (x) } )
#define __pgprot(x)	((pgprot_t) { (x) } )

/* to align the pointer to the (next) page boundary */
/*Defined in linux/mm.h*/
/*#define PAGE_ALIGN(addr)	(((addr)+PAGE_SIZE-1)&PAGE_MASK)*/

extern unsigned long m68k_memoffset;

#define WANT_PAGE_VIRTUAL

extern unsigned long cf_dma_base;
extern unsigned long cf_dma_end;

/*
 * Convert a virt to a phys
 */
static inline unsigned long ___pa(void *vaddr)
{
#if CONFIG_SDRAM_BASE != PAGE_OFFSET
	return (((unsigned long)vaddr & 0x0fffffff) + CONFIG_SDRAM_BASE);
#else
	if ((unsigned long)vaddr >= CONFIG_DMA_BASE &&
	    (unsigned long)vaddr < (CONFIG_DMA_BASE + CONFIG_DMA_SIZE)) {
		/* address is in carved out DMA range */
		return ((unsigned long)vaddr - CONFIG_DMA_BASE) + CONFIG_SDRAM_BASE;
	}
	else if ((unsigned long)vaddr >= PAGE_OFFSET &&
		 (unsigned long)vaddr < (PAGE_OFFSET + CONFIG_SDRAM_SIZE)) {
		/* normal mapping */
		return ((unsigned long)vaddr - PAGE_OFFSET) + CONFIG_SDRAM_BASE;
	}

	return (unsigned long)vaddr;
#endif
}
#define __pa(vaddr)	___pa((void *)(vaddr))

/*
 * Convert a phys to a virt
 */
static inline void *__va(unsigned long paddr)
{
#if CONFIG_SDRAM_BASE != PAGE_OFFSET
	return (void *)((paddr & 0x0fffffff) + PAGE_OFFSET);
#else
	if (paddr >= cf_dma_base && paddr <= cf_dma_end) {
		/* mapped address for DMA */
		return (void *)((paddr - CONFIG_SDRAM_BASE) + CONFIG_DMA_BASE);
	}
	else if (paddr >= cf_dma_end &&
		 paddr < (CONFIG_SDRAM_BASE + CONFIG_SDRAM_SIZE)) {
		/* normal mapping */
		return (void *)((paddr - CONFIG_SDRAM_BASE) + PAGE_OFFSET);
	}
	return (void *)paddr;
#endif
}

/*
 * NOTE: virtual isn't really correct, actually it should be the offset into the
 * memory node, but we have no highmem, so that works for now.
 *
 * TODO: implement (fast) pfn<->pgdat_idx conversion functions, this makes lots
 * of the shifts unnecessary.
 *
 * PFNs are used to map physical pages.  So PFN[0] maps to the base phys addr.
 */
#define virt_to_pfn(kaddr)	(__pa(kaddr) >> PAGE_SHIFT)
#define pfn_to_virt(pfn)	__va((pfn) << PAGE_SHIFT)

extern int m68k_virt_to_node_shift;

#ifdef CONFIG_SINGLE_MEMORY_CHUNK
#define __virt_to_node(addr)	(&pg_data_map[0])
#else
extern struct pglist_data *pg_data_table[];

static inline __attribute_const__ int __virt_to_node_shift(void)
{
	return m68k_virt_to_node_shift;
}

#define __virt_to_node(addr)	(pg_data_table[(unsigned long)(addr) >> __virt_to_node_shift()])
#endif /* !CONFIG_SINGLE_MEMORY_CHUNK */

#define virt_to_page(addr) ({						\
	pfn_to_page(virt_to_pfn(addr));					\
})
#define page_to_virt(page) ({						\
	pfn_to_virt(page_to_pfn(page));					\
})

#define pfn_to_page(pfn) ({						\
	unsigned long __pfn = (pfn);					\
	struct pglist_data *pgdat;					\
	pgdat = __virt_to_node((unsigned long)pfn_to_virt(__pfn));	\
	pgdat->node_mem_map + (__pfn - pgdat->node_start_pfn);		\
})
#define page_to_pfn(_page) ({						\
	struct page *__p = (_page);					\
	struct pglist_data *pgdat;					\
	pgdat = &pg_data_map[page_to_nid(__p)];				\
	((__p) - pgdat->node_mem_map) + pgdat->node_start_pfn;		\
})

#define virt_addr_valid(kaddr)	( ((void *)(kaddr) >= (void *)PAGE_OFFSET && \
				(void *)(kaddr) < high_memory) || \
				((void *)(kaddr) >= (void*)CONFIG_DMA_BASE && \
				(void *)(kaddr) < (void*)(CONFIG_DMA_BASE+CONFIG_DMA_SIZE)))

#define pfn_valid(pfn)		virt_addr_valid(pfn_to_virt(pfn))

#endif /* __ASSEMBLY__ */

#define VM_DATA_DEFAULT_FLAGS	(VM_READ | VM_WRITE | VM_EXEC | \
				 VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC)

#include <asm-generic/getorder.h>

#ifdef CONFIG_VDSO
#define __HAVE_ARCH_GATE_AREA
#endif

#endif /* __CF_PAGE__ */
