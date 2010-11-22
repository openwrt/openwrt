/*
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 */
#ifndef M68K_CF_TLBFLUSH_H
#define M68K_CF_TLBFLUSH_H

#include <asm/coldfire.h>

/* Flush all userspace mappings.  */
static inline void flush_tlb_all(void)
{
	preempt_disable();
	*MMUOR = MMUOR_CNL;
	preempt_enable();
}

/* Clear user TLB entries within the context named in mm */
static inline void flush_tlb_mm(struct mm_struct *mm)
{
	preempt_disable();
	*MMUOR = MMUOR_CNL;
	preempt_enable();
}

/* Flush a single TLB page.  */
static inline void flush_tlb_page(struct vm_area_struct *vma,
				   unsigned long addr)
{
	preempt_disable();
	*MMUOR = MMUOR_CNL;
	preempt_enable();
}
/* Flush a range of pages from TLB. */

static inline void flush_tlb_range(struct mm_struct *mm,
		      unsigned long start, unsigned long end)
{
	preempt_disable();
	*MMUOR = MMUOR_CNL;
	preempt_enable();
}

/* Flush kernel page from TLB. */
static inline void flush_tlb_kernel_page(void *addr)
{
	preempt_disable();
	*MMUOR = MMUOR_CNL;
	preempt_enable();
}

static inline void flush_tlb_kernel_range(unsigned long start,
	unsigned long end)
{
	flush_tlb_all();
}

extern inline void flush_tlb_pgtables(struct mm_struct *mm,
				      unsigned long start, unsigned long end)
{
}

#endif /* M68K_CF_TLBFLUSH_H */
