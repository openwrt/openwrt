/*
 * arch/ubicom32/mach-common/cachectl.c
 *   Architecture cache control support
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

#include <linux/types.h>
#include <linux/module.h>
#include <asm/cachectl.h>

/*
 * The write queue flush procedure in mem_cache_control needs to make
 * DCACHE_WRITE_QUEUE_LENGTH writes to DDR (not OCM). Here we reserve some
 * memory for this operation.
 * Allocate array of cache lines of least DCACHE_WRITE_QUEUE_LENGTH + 1 words in
 * length rounded up to the nearest cache line.
 */
#define CACHE_WRITE_QUEUE_FLUSH_AREA_SIZE \
	ALIGN(sizeof(int) * (DCACHE_WRITE_QUEUE_LENGTH + 1), CACHE_LINE_SIZE)

static char cache_write_queue_flush_area[CACHE_WRITE_QUEUE_FLUSH_AREA_SIZE]
	__attribute__((aligned(CACHE_LINE_SIZE)));

/*
 * ONE_CCR_ADDR_OP is a helper macro that executes a single CCR operation.
 */
#define ONE_CCR_ADDR_OP(cc, op_addr, op)				\
	do {								\
		asm volatile (						\
		"	btst	"D(CCR_CTRL)"(%0), #"D(CCR_CTRL_VALID)"				\n\t" \
		"	jmpne.f	.-4								\n\t" \
		"	move.4	"D(CCR_ADDR)"(%0), %1						\n\t" \
		"	move.1	"D(CCR_CTRL+3)"(%0), %2						\n\t" \
		"	bset	"D(CCR_CTRL)"(%0), "D(CCR_CTRL)"(%0), #"D(CCR_CTRL_VALID)"	\n\t" \
		"	cycles	2								\n\t" \
		"	btst	"D(CCR_CTRL)"(%0), #"D(CCR_CTRL_DONE)"				\n\t" \
		"	jmpeq.f	.-4								\n\t" \
			:						\
			: "a"(cc), "r"(op_addr), "r"(op & 0xff)		\
			: "cc"						\
		);							\
	} while (0)

/*
 * mem_cache_control()
 *	Special cache control operation
 */
void mem_cache_control(unsigned long cc, unsigned long begin_addr,
		       unsigned long end_addr, unsigned long op)
{
	unsigned long op_addr;
	int dccr = cc == DCCR_BASE;
	if (dccr && op == CCR_CTRL_FLUSH_ADDR) {
		/*
		 * We ensure all previous writes have left the data cache write
		 * queue by sending DCACHE_WRITE_QUEUE_LENGTH writes (to
		 * different words) down the queue.  If this is not done it's
		 * possible that the data we are trying to flush hasn't even
		 * entered the data cache.
		 * The +1 ensure that the final 'flush' is actually a flush.
		 */
		int *flush_area = (int *)cache_write_queue_flush_area;
		asm volatile(
			"	.rept "D(DCACHE_WRITE_QUEUE_LENGTH + 1)"	\n\t"
			"	move.4 (%0)4++, d0				\n\t"
			"	.endr						\n\t"
			: "+a"(flush_area)
			);
	}

	if (dccr)
		UBICOM32_LOCK(DCCR_LOCK_BIT);
	else
		UBICOM32_LOCK(ICCR_LOCK_BIT);

	/*
	 * Calculate the cache lines we need to operate on that include
	 * begin_addr though end_addr.
	 */
	begin_addr = begin_addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = (end_addr + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
	op_addr = begin_addr;

	do {
		ONE_CCR_ADDR_OP(cc, op_addr, op);
		op_addr += CACHE_LINE_SIZE;
	} while (likely(op_addr < end_addr));

	if (dccr && op == CCR_CTRL_FLUSH_ADDR) {
		/*
		 * It turns out that when flushing the data cache the last flush
		 * isn't actually complete at this point. This is because there
		 * is another write buffer on the DDR side of the cache that is
		 * arbitrated with the I-Cache.
		 *
		 * The only foolproof method that ensures that the last data
		 * cache flush *actually* completed is to do another flush on a
		 * dirty cache line. This flush will block until the DDR write
		 * buffer is empty.
		 *
		 * Rather than creating a another dirty cache line, we use the
		 * flush_area above as we know that it is dirty from previous
		 * writes.
		 */
		ONE_CCR_ADDR_OP(cc, cache_write_queue_flush_area, op);
	}

	if (dccr)
		UBICOM32_UNLOCK(DCCR_LOCK_BIT);
	else
		UBICOM32_UNLOCK(ICCR_LOCK_BIT);

}
EXPORT_SYMBOL(mem_cache_control);
