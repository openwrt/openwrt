/*
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 */

#ifndef _CF_PGTABLE_H
#define _CF_PGTABLE_H

#include <asm/cfmmu.h>
#include <asm/page.h>

#ifndef __ASSEMBLY__
#include <asm/virtconvert.h>
#include <linux/linkage.h>

/* For virtual address to physical address conversion */
#define VTOP(addr)	__pa(addr)
#define PTOV(addr)	__va(addr)


#endif	/* !__ASSEMBLY__ */

/* Page protection values within PTE. */

/* MMUDR bits, in proper place. */
#define CF_PAGE_LOCKED      (0x00000002)
#define CF_PAGE_EXEC        (0x00000004)
#define CF_PAGE_WRITABLE    (0x00000008)
#define CF_PAGE_READABLE    (0x00000010)
#define CF_PAGE_SYSTEM      (0x00000020)
#define CF_PAGE_COPYBACK    (0x00000040)
#define CF_PAGE_NOCACHE     (0x00000080)

#define CF_CACHEMASK       (~0x00000040)
#define CF_PAGE_MMUDR_MASK  (0x000000fe)

#define _PAGE_NOCACHE030  (CF_PAGE_NOCACHE)

/* MMUTR bits, need shifting down.  */
#define CF_PAGE_VALID       (0x00000400)
#define CF_PAGE_SHARED      (0x00000800)

#define CF_PAGE_MMUTR_MASK  (0x00000c00)
#define CF_PAGE_MMUTR_SHIFT (10)
#define CF_ASID_MMU_SHIFT   (2)

/* Fake bits, not implemented in CF, will get masked out before
   hitting hardware, and might go away altogether once this port is
   complete.  */
#if PAGE_SHIFT < 13
#error COLDFIRE Error: Pages must be at least 8k in size
#endif
#define CF_PAGE_ACCESSED    (0x00001000)
#define CF_PAGE_FILE        (0x00000200)
#define CF_PAGE_DIRTY       (0x00000001)

#define _PAGE_CACHE040	0x020   /* 68040 cache mode, cachable, copyback */
#define _PAGE_NOCACHE_S 0x040   /* 68040 no-cache mode, serialized */
#define _PAGE_NOCACHE   0x060   /* 68040 cache mode, non-serialized */
#define _PAGE_CACHE040W 0x000   /* 68040 cache mode, cachable, write-through */
#define _DESCTYPE_MASK  0x003
#define _CACHEMASK040   (~0x060)
#define _PAGE_GLOBAL040 0x400   /* 68040 global bit, used for kva descs */


/* Externally used page protection values. */
#define _PAGE_PRESENT	(CF_PAGE_VALID)
#define _PAGE_ACCESSED	(CF_PAGE_ACCESSED)
#define _PAGE_DIRTY	(CF_PAGE_DIRTY)
#define _PAGE_READWRITE (CF_PAGE_WRITABLE \
			| CF_PAGE_READABLE \
			| CF_PAGE_SHARED \
			| CF_PAGE_SYSTEM)

/* Compound page protection values. */
#define PAGE_NONE	__pgprot(CF_PAGE_VALID \
				 | CF_PAGE_ACCESSED)

#define PAGE_SHARED     __pgprot(CF_PAGE_VALID \
				 | CF_PAGE_READABLE \
				 | CF_PAGE_WRITABLE \
				 | CF_PAGE_ACCESSED)

#define PAGE_INIT	__pgprot(CF_PAGE_VALID \
				 | CF_PAGE_WRITABLE \
				 | CF_PAGE_READABLE \
				 | CF_PAGE_EXEC \
				 | CF_PAGE_SYSTEM \
				 | CF_PAGE_SHARED)

#define PAGE_KERNEL	__pgprot(CF_PAGE_VALID \
				 | CF_PAGE_WRITABLE \
				 | CF_PAGE_READABLE \
				 | CF_PAGE_EXEC \
				 | CF_PAGE_SYSTEM \
				 | CF_PAGE_SHARED \
				 | CF_PAGE_ACCESSED)

#define PAGE_COPY	__pgprot(CF_PAGE_VALID \
				 | CF_PAGE_ACCESSED \
				 | CF_PAGE_READABLE)

/*
 * Page protections for initialising protection_map.  See mm/mmap.c
 * for use.  In general, the bit positions are xwr, and P-items are
 * private, the S-items are shared.
 */

#define __P000	PAGE_NONE
#define __P100	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_EXEC)
#define __P010	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_WRITABLE \
			 | CF_PAGE_ACCESSED)
#define __P110	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_WRITABLE \
			 | CF_PAGE_EXEC)
#define __P001	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_READABLE)
#define __P101	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_READABLE \
			 | CF_PAGE_EXEC)
#define __P011	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_READABLE \
			 | CF_PAGE_WRITABLE \
			 | CF_PAGE_ACCESSED)
#define __P111	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_WRITABLE \
			 | CF_PAGE_READABLE \
			 | CF_PAGE_EXEC)

#define __S000	PAGE_NONE
#define __S100	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_EXEC)
#define __S010	PAGE_SHARED
#define __S110	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_WRITABLE \
			 | CF_PAGE_EXEC)
#define __S001	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_READABLE)
#define __S101	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_READABLE \
			 | CF_PAGE_EXEC)
#define __S011	PAGE_SHARED
#define __S111	__pgprot(CF_PAGE_VALID \
			 | CF_PAGE_ACCESSED \
			 | CF_PAGE_READABLE \
			 | CF_PAGE_WRITABLE \
			 | CF_PAGE_EXEC)

#define PTE_MASK	PAGE_MASK
#define CF_PAGE_CHG_MASK	(PTE_MASK | CF_PAGE_ACCESSED | CF_PAGE_DIRTY)

#ifndef __ASSEMBLY__

/*
 * Conversion functions: convert a page and protection to a page entry,
 * and a page entry and page directory to the page they refer to.
 */
#define mk_pte(page, pgprot) pfn_pte(page_to_pfn(page), (pgprot))

extern inline pte_t pte_modify(pte_t pte, pgprot_t newprot)
{
	pte_val(pte) = (pte_val(pte) & CF_PAGE_CHG_MASK) | pgprot_val(newprot);
	return pte;
}

#define pmd_set(pmdp, ptep) do {} while (0)

static inline void pgd_set(pgd_t *pgdp, pmd_t *pmdp)
{
	pgd_val(*pgdp) = virt_to_phys(pmdp);
}

#define __pte_page(pte) \
	((unsigned long) ((pte_val(pte) & CF_PAGE_PGNUM_MASK) + PAGE_OFFSET))
#define __pmd_page(pmd) ((unsigned long) (pmd_val(pmd)))

extern inline int pte_none(pte_t pte)
{
	return !pte_val(pte);
}
extern inline int pte_present(pte_t pte)
{
	return pte_val(pte) & CF_PAGE_VALID;
}
extern inline void pte_clear(struct mm_struct *mm, unsigned long addr,
	pte_t *ptep)
{
	pte_val(*ptep) = 0;
}

#define pte_pagenr(pte)		((__pte_page(pte) - PAGE_OFFSET) >> PAGE_SHIFT)
#define pte_page(pte)		virt_to_page(__pte_page(pte))

extern inline int pmd_none2(pmd_t *pmd) { return !pmd_val(*pmd); }
#define pmd_none(pmd) pmd_none2(&(pmd))
extern inline int pmd_bad2(pmd_t *pmd) { return 0; }
#define pmd_bad(pmd) pmd_bad2(&(pmd))
#define pmd_present(pmd) (!pmd_none2(&(pmd)))
extern inline void pmd_clear(pmd_t *pmdp) { pmd_val(*pmdp) = 0; }

extern inline int pgd_none(pgd_t pgd) { return 0; }
extern inline int pgd_bad(pgd_t pgd) { return 0; }
extern inline int pgd_present(pgd_t pgd) { return 1; }
extern inline void pgd_clear(pgd_t *pgdp) {}


#define pte_ERROR(e) \
	printk(KERN_ERR "%s:%d: bad pte %08lx.\n",	\
	__FILE__, __LINE__, pte_val(e))
#define pmd_ERROR(e) \
	printk(KERN_ERR "%s:%d: bad pmd %08lx.\n",	\
	__FILE__, __LINE__, pmd_val(e))
#define pgd_ERROR(e) \
	printk(KERN_ERR "%s:%d: bad pgd %08lx.\n",	\
	__FILE__, __LINE__, pgd_val(e))


/*
 * The following only work if pte_present() is true.
 * Undefined behaviour if not...
 * [we have the full set here even if they don't change from m68k]
 */
extern inline int pte_read(pte_t pte)	\
	{ return pte_val(pte) & CF_PAGE_READABLE; }
extern inline int pte_write(pte_t pte)	\
	{ return pte_val(pte) & CF_PAGE_WRITABLE; }
extern inline int pte_exec(pte_t pte)	\
	{ return pte_val(pte) & CF_PAGE_EXEC; }
extern inline int pte_dirty(pte_t pte)	\
	{ return pte_val(pte) & CF_PAGE_DIRTY; }
extern inline int pte_young(pte_t pte)	\
	{ return pte_val(pte) & CF_PAGE_ACCESSED; }
extern inline int pte_file(pte_t pte)	\
	{ return pte_val(pte) & CF_PAGE_FILE; }
static inline int pte_special(pte_t pte)	{ return 0; }


extern inline pte_t pte_wrprotect(pte_t pte)	\
	{ pte_val(pte) &= ~CF_PAGE_WRITABLE; return pte; }
extern inline pte_t pte_rdprotect(pte_t pte)	\
	{ pte_val(pte) &= ~CF_PAGE_READABLE; return pte; }
extern inline pte_t pte_exprotect(pte_t pte)	\
	{ pte_val(pte) &= ~CF_PAGE_EXEC; return pte; }
extern inline pte_t pte_mkclean(pte_t pte)	\
	{ pte_val(pte) &= ~CF_PAGE_DIRTY; return pte; }
extern inline pte_t pte_mkold(pte_t pte)	\
	{ pte_val(pte) &= ~CF_PAGE_ACCESSED; return pte; }
extern inline pte_t pte_mkwrite(pte_t pte)	\
	{ pte_val(pte) |= CF_PAGE_WRITABLE; return pte; }
extern inline pte_t pte_mkread(pte_t pte)	\
	{ pte_val(pte) |= CF_PAGE_READABLE; return pte; }
extern inline pte_t pte_mkexec(pte_t pte)	\
	{ pte_val(pte) |= CF_PAGE_EXEC; return pte; }
extern inline pte_t pte_mkdirty(pte_t pte)	\
	{ pte_val(pte) |= CF_PAGE_DIRTY; return pte; }
extern inline pte_t pte_mkyoung(pte_t pte)	\
	{ pte_val(pte) |= CF_PAGE_ACCESSED; return pte; }
extern inline pte_t pte_mknocache(pte_t pte)	\
	{ pte_val(pte) |= 0x80 | (pte_val(pte) & ~0x40); return pte; }
extern inline pte_t pte_mkcache(pte_t pte)	\
	{ pte_val(pte) &= ~CF_PAGE_NOCACHE; return pte; }
static inline pte_t pte_mkspecial(pte_t pte)	{ return pte; }


#define swapper_pg_dir kernel_pg_dir
extern pgd_t kernel_pg_dir[PTRS_PER_PGD];

/* Find an entry in a pagetable directory. */
#define pgd_index(address)     ((address) >> PGDIR_SHIFT)

#define pgd_offset(mm, address) ((mm)->pgd + pgd_index(address))

/* Find an entry in a kernel pagetable directory. */
#define pgd_offset_k(address) pgd_offset(&init_mm, address)

/* Find an entry in the second-level pagetable. */
extern inline pmd_t *pmd_offset(pgd_t *pgd, unsigned long address)
{
	return (pmd_t *) pgd;
}

/* Find an entry in the third-level pagetable. */
#define __pte_offset(address) ((address >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_offset_kernel(dir, address) ((pte_t *) __pmd_page(*(dir)) + \
					  __pte_offset(address))

/* Disable caching for page at given kernel virtual address. */
static inline void nocache_page(void *vaddr)
{
	pgd_t *dir;
	pmd_t *pmdp;
	pte_t *ptep;
	unsigned long addr = (unsigned long)vaddr;

	dir = pgd_offset_k(addr);
	pmdp = pmd_offset(dir, addr);
	ptep = pte_offset_kernel(pmdp, addr);
	*ptep = pte_mknocache(*ptep);
}

/* Enable caching for page at given kernel virtual address. */
static inline void cache_page(void *vaddr)
{
	pgd_t *dir;
	pmd_t *pmdp;
	pte_t *ptep;
	unsigned long addr = (unsigned long)vaddr;

	dir = pgd_offset_k(addr);
	pmdp = pmd_offset(dir, addr);
	ptep = pte_offset_kernel(pmdp, addr);
	*ptep = pte_mkcache(*ptep);
}

#define PTE_FILE_MAX_BITS	21
#define PTE_FILE_SHIFT		11

static inline unsigned long pte_to_pgoff(pte_t pte)
{
	return pte_val(pte) >> PTE_FILE_SHIFT;
}

static inline pte_t pgoff_to_pte(unsigned pgoff)
{
	pte_t pte = __pte((pgoff << PTE_FILE_SHIFT) + CF_PAGE_FILE);
	return pte;
}

/* Encode and de-code a swap entry (must be !pte_none(e) && !pte_present(e)) */
#define __swp_entry(type, offset) ((swp_entry_t) { (type) |	\
				   (offset << PTE_FILE_SHIFT) })
#define __swp_type(x)		((x).val & 0xFF)
#define __swp_offset(x)		((x).val >> PTE_FILE_SHIFT)
#define __pte_to_swp_entry(pte)	((swp_entry_t) { pte_val(pte) })
#define __swp_entry_to_pte(x)	(__pte((x).val))

#define pmd_page(pmd)		(pfn_to_page(pmd_val(pmd) >> PAGE_SHIFT))

#define pte_offset_map(pmdp, address) ((pte_t *)__pmd_page(*pmdp) +	\
				       __pte_offset(address))
#define pte_offset_map_nested(pmdp, address) pte_offset_map(pmdp, address)
#define pte_unmap(pte) kunmap(pte)
#define pte_unmap_nested(pte) kunmap(pte)

#define pfn_pte(pfn, prot)	__pte(((pfn) << PAGE_SHIFT) | pgprot_val(prot))
#define pte_pfn(pte)		(pte_val(pte) >> PAGE_SHIFT)


#endif	/* !__ASSEMBLY__ */
#endif	/* !_CF_PGTABLE_H */
