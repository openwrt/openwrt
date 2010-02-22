/*
 * arch/ubicom32/include/asm/cacheflush.h
 *   Cache flushing definitions for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
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
#ifndef _ASM_UBICOM32_CACHEFLUSH_H
#define _ASM_UBICOM32_CACHEFLUSH_H

/*
 * (C) Copyright 2000-2004, Greg Ungerer <gerg@snapgear.com>
 */
#include <linux/mm.h>
#include <asm/cachectl.h>
#include <asm/ip5000.h>

#define flush_cache_all()			__flush_cache_all()
#define flush_cache_mm(mm)			do { } while (0)
#define flush_cache_dup_mm(mm)			do { } while (0)
#define flush_cache_range(vma, start, end)	__flush_cache_all()
#define flush_cache_page(vma, vmaddr)		do { } while (0)
#define flush_dcache_page(page)			do { } while (0)
#define flush_dcache_mmap_lock(mapping)		do { } while (0)
#define flush_dcache_mmap_unlock(mapping)	do { } while (0)

#define flush_dcache_range(start, end)					\
do {									\
	/* Flush the data cache and invalidate the I cache. */		\
	mem_cache_control(DCCR_BASE, start, end, CCR_CTRL_FLUSH_ADDR);	\
	mem_cache_control(ICCR_BASE, start, end, CCR_CTRL_INV_ADDR);	\
} while (0)

#define flush_icache_range(start, end)					\
do {									\
	/* Flush the data cache and invalidate the I cache. */		\
	mem_cache_control(DCCR_BASE, start, end, CCR_CTRL_FLUSH_ADDR);	\
	mem_cache_control(ICCR_BASE, start, end, CCR_CTRL_INV_ADDR);	\
} while (0)

#define flush_icache_page(vma,pg)		do { } while (0)
#define flush_icache_user_range(vma,pg,adr,len)	do { } while (0)
#define flush_cache_vmap(start, end)		do { } while (0)
#define flush_cache_vunmap(start, end)		do { } while (0)

#define copy_to_user_page(vma, page, vaddr, dst, src, len) \
	memcpy(dst, src, len)
#define copy_from_user_page(vma, page, vaddr, dst, src, len) \
	memcpy(dst, src, len)

/*
 * Cache handling for IP5000
 */
extern inline void mem_cache_invalidate_all(unsigned long cc)
{
	if (cc == DCCR_BASE)
		UBICOM32_LOCK(DCCR_LOCK_BIT);
	else
		UBICOM32_LOCK(ICCR_LOCK_BIT);

	asm volatile (
	"	bset	"D(CCR_CTRL)"(%0), "D(CCR_CTRL)"(%0), #"D(CCR_CTRL_RESET)"	\n\t"
	"	nop									\n\t"
	"	bclr	"D(CCR_CTRL)"(%0), "D(CCR_CTRL)"(%0), #"D(CCR_CTRL_RESET)"	\n\t"
	"	pipe_flush 0								\n\t"
		:
		: "a"(cc)
		: "cc"
	);

	if (cc == DCCR_BASE)
		UBICOM32_UNLOCK(DCCR_LOCK_BIT);
	else
		UBICOM32_UNLOCK(ICCR_LOCK_BIT);

}

static inline void __flush_cache_all(void)
{
	/*
	 * Flush Icache
	 */
	mem_cache_invalidate_all(ICCR_BASE);

	/*
	 * Flush Dcache
	 */
	mem_cache_invalidate_all(DCCR_BASE);
}

#endif /* _ASM_UBICOM32_CACHEFLUSH_H */
