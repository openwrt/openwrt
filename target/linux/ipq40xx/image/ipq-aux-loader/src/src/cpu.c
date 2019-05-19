/*
 * Auxiliary kernel loader for Qualcom IPQ-4XXX/806X based boards
 *
 * Copyright (C) 2019 Sergey Sergeev <adron@mstnt.com>
 *
 * Some structures and code has been taken from the U-Boot project.
 *	(C) Copyright 2008 Semihalf
 *	(C) Copyright 2000-2005
 *	Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <types.h>

/* size: 4 * 4096. Start: TEXT_BASE + 40M. Try to limit to 128M.
	 !!! This region of memory should not be inside the caching area !!!
*/
static u32 *page_table = (void*)(CONFIG_SYS_TEXT_BASE + 0x2800000);

/* start and size of memory for D-Cache */
static u32 bi_dram_0_start = 0;
static u32 bi_dram_0_size = 0;

#define CR_C	(1 << 2)	/* Dcache enable			*/
#define CR_I	(1 << 12)	/* Icache enable			*/
#define CR_M	(1 << 0)	/* MMU enable				*/

#define CONFIG_NR_DRAM_BANKS		1
#if defined(CONFIG_SYS_ARM_CACHE_WRITETHROUGH)
#define CACHE_SETUP	0x1a
#else
#define CACHE_SETUP	0x1e
#endif

#define isb() __asm__ __volatile__ ("" : : : "memory")
#define nop() __asm__ __volatile__("mov\tr0,r0\t@ nop\n\t");

static void cp_delay (void)
{
	volatile int i;

	/* copro seems to need some delay between reading and writing */
	for (i = 0; i < 100; i++)
		nop();
	asm volatile("" : : : "memory");
}

static inline unsigned int get_cr(void)
{
	unsigned int val;
	asm("mrc p15, 0, %0, c1, c0, 0	@ get CR" : "=r" (val) : : "cc");
	return val;
}

static inline void set_cr(unsigned int val)
{
	asm volatile("mcr p15, 0, %0, c1, c0, 0	@ set CR"
	  : : "r" (val) : "cc");
	isb();
}

/* Values for Ctype fields in CLIDR */
#define ARMV7_CLIDR_CTYPE_NO_CACHE		0
#define ARMV7_CLIDR_CTYPE_INSTRUCTION_ONLY	1
#define ARMV7_CLIDR_CTYPE_DATA_ONLY		2
#define ARMV7_CLIDR_CTYPE_INSTRUCTION_DATA	3
#define ARMV7_CLIDR_CTYPE_UNIFIED		4

#define ARMV7_DCACHE_INVAL_ALL		1
#define ARMV7_DCACHE_CLEAN_INVAL_ALL	2
#define ARMV7_DCACHE_INVAL_RANGE	3
#define ARMV7_DCACHE_CLEAN_INVAL_RANGE	4

#define ARMV7_CSSELR_IND_DATA_UNIFIED	0

/* CCSIDR */
#define CCSIDR_LINE_SIZE_OFFSET		0
#define CCSIDR_LINE_SIZE_MASK		0x7
#define CCSIDR_ASSOCIATIVITY_OFFSET	3
#define CCSIDR_ASSOCIATIVITY_MASK	(0x3FF << 3)
#define CCSIDR_NUM_SETS_OFFSET		13
#define CCSIDR_NUM_SETS_MASK		(0x7FFF << 13)

/*
 * CP15 Barrier instructions
 * Please note that we have separate barrier instructions in ARMv7
 * However, we use the CP15 based instructtions because we use
 * -march=armv5 in U-Boot
 */
#define CP15ISB	asm volatile ("mcr     p15, 0, %0, c7, c5, 4" : : "r" (0))
#define CP15DSB	asm volatile ("mcr     p15, 0, %0, c7, c10, 4" : : "r" (0))
#define CP15DMB	asm volatile ("mcr     p15, 0, %0, c7, c10, 5" : : "r" (0))

static inline s32 log_2_n_round_up(u32 n)
{
	s32 log2n = -1;
	u32 temp = n;

	while (temp) {
		log2n++;
		temp >>= 1;
	}

	if (n & (n - 1))
		return log2n + 1; /* not power of 2 - round up */
	else
		return log2n; /* power of 2 */
}

/*
 * Write the level and type you want to Cache Size Selection Register(CSSELR)
 * to get size details from Current Cache Size ID Register(CCSIDR)
 */
static void set_csselr(u32 level, u32 type)
{	u32 csselr = level << 1 | type;

	/* Write to Cache Size Selection Register(CSSELR) */
	asm volatile ("mcr p15, 2, %0, c0, c0, 0" : : "r" (csselr));
}

static u32 get_ccsidr(void)
{
	u32 ccsidr;

	/* Read current CP15 Cache Size ID Register */
	asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (ccsidr));
	return ccsidr;
}


static u32 get_clidr(void)
{
	u32 clidr;

	/* Read current CP15 Cache Level ID Register */
	asm volatile ("mrc p15,1,%0,c0,c0,1" : "=r" (clidr));
	return clidr;
}

static void v7_inval_dcache_level_setway(u32 level, u32 num_sets,
					 u32 num_ways, u32 way_shift,
					 u32 log2_line_len)
{
	int way, set, setway;

	/*
	 * For optimal assembly code:
	 *	a. count down
	 *	b. have bigger loop inside
	 */
	for (way = num_ways - 1; way >= 0 ; way--) {
		for (set = num_sets - 1; set >= 0; set--) {
			setway = (level << 1) | (set << log2_line_len) |
				 (way << way_shift);
			/* Invalidate data/unified cache line by set/way */
			asm volatile ("	mcr p15, 0, %0, c7, c6, 2"
					: : "r" (setway));
		}
	}
	/* DSB to make sure the operation is complete */
	CP15DSB;
}

static void v7_clean_inval_dcache_level_setway(u32 level, u32 num_sets,
					       u32 num_ways, u32 way_shift,
					       u32 log2_line_len)
{
	int way, set, setway;

	/*
	 * For optimal assembly code:
	 *	a. count down
	 *	b. have bigger loop inside
	 */
	for (way = num_ways - 1; way >= 0 ; way--) {
		for (set = num_sets - 1; set >= 0; set--) {
			setway = (level << 1) | (set << log2_line_len) |
				 (way << way_shift);
			/*
			 * Clean & Invalidate data/unified
			 * cache line by set/way
			 */
			asm volatile ("	mcr p15, 0, %0, c7, c14, 2"
					: : "r" (setway));
		}
	}
	/* DSB to make sure the operation is complete */
	CP15DSB;
}

static void v7_maint_dcache_level_setway(u32 level, u32 operation)
{
	u32 ccsidr;
	u32 num_sets, num_ways, log2_line_len, log2_num_ways;
	u32 way_shift;

	set_csselr(level, ARMV7_CSSELR_IND_DATA_UNIFIED);

	ccsidr = get_ccsidr();

	log2_line_len = ((ccsidr & CCSIDR_LINE_SIZE_MASK) >>
				CCSIDR_LINE_SIZE_OFFSET) + 2;
	/* Converting from words to bytes */
	log2_line_len += 2;

	num_ways  = ((ccsidr & CCSIDR_ASSOCIATIVITY_MASK) >>
			CCSIDR_ASSOCIATIVITY_OFFSET) + 1;
	num_sets  = ((ccsidr & CCSIDR_NUM_SETS_MASK) >>
			CCSIDR_NUM_SETS_OFFSET) + 1;
	/*
	 * According to ARMv7 ARM number of sets and number of ways need
	 * not be a power of 2
	 */
	log2_num_ways = log_2_n_round_up(num_ways);

	way_shift = (32 - log2_num_ways);
	if (operation == ARMV7_DCACHE_INVAL_ALL) {
		v7_inval_dcache_level_setway(level, num_sets, num_ways,
				      way_shift, log2_line_len);
	} else if (operation == ARMV7_DCACHE_CLEAN_INVAL_ALL) {
		v7_clean_inval_dcache_level_setway(level, num_sets, num_ways,
						   way_shift, log2_line_len);
	}
}


static void v7_maint_dcache_all(u32 operation)
{
	u32 level, cache_type, level_start_bit = 0;

	u32 clidr = get_clidr();

	for (level = 0; level < 7; level++) {
		cache_type = (clidr >> level_start_bit) & 0x7;
		if ((cache_type == ARMV7_CLIDR_CTYPE_DATA_ONLY) ||
		    (cache_type == ARMV7_CLIDR_CTYPE_INSTRUCTION_DATA) ||
		    (cache_type == ARMV7_CLIDR_CTYPE_UNIFIED))
			v7_maint_dcache_level_setway(level, operation);
		level_start_bit += 3;
	}
}

void __v7_outer_cache_flush_all(void)
{
}
void v7_outer_cache_flush_all(void)
	__attribute__((weak, alias("__v7_outer_cache_flush_all")));

void __v7_outer_cache_disable(void)
{
}
void v7_outer_cache_disable(void)
	__attribute__((weak, alias("__v7_outer_cache_disable")));

void __v7_outer_cache_inval_all(void)
{
}
void v7_outer_cache_inval_all(void)
	__attribute__((weak, alias("__v7_outer_cache_inval_all")));

void set_l2_indirect_reg(u32 reg_addr, u32 val)
{

	asm volatile ("mcr     p15, 3, %[l2cpselr], c15, c0, 6\n\t"
		      "isb\n\t"
		      "mcr     p15, 3, %[l2cpdr],   c15, c0, 7\n\t"
		      "isb\n\t"
			:
			: [l2cpselr]"r" (reg_addr), [l2cpdr]"r" (val)
	);
}

u32 get_l2_indirect_reg(u32 reg_addr)
{
	u32 val;

	asm volatile ("mcr     p15, 3, %[l2cpselr], c15, c0, 6\n\t"
		      "isb\n\t"
		      "mrc     p15, 3, %[l2cpdr],   c15, c0, 7\n\t"
			: [l2cpdr]"=r" (val)
			: [l2cpselr]"r" (reg_addr)
	);

	return val;
}

void clear_l2cache_err(void)
{
#if defined CONFIG_IPQ806X
#define L2ESR_IND_ADDR		(0x204)
	unsigned int val;

	val = get_l2_indirect_reg(L2ESR_IND_ADDR);
	set_l2_indirect_reg(L2ESR_IND_ADDR, val);
#endif
}

void __arm_init_before_mmu(void)
{
}
void arm_init_before_mmu(void)
	__attribute__((weak, alias("__arm_init_before_mmu")));

void bi_dram_0_set_ranges(u32 start, u32 size){
	bi_dram_0_start = start;
	bi_dram_0_size = size;
}

static inline void dram_bank_mmu_setup(int bank)
{
	u32	i;
	u32 skip = (u32)page_table >> 20;
	for (i = bi_dram_0_start >> 20;
	     i < (bi_dram_0_start + bi_dram_0_size) >> 20;
	     i++) {
	  if(i == skip){ /* foolproof */
	  	continue;
	  }
		page_table[i] = i << 20 | (3 << 10) | CACHE_SETUP;
	}
}

/* to activate the MMU we need to set up virtual memory: use 1M areas */
static inline void mmu_setup(void)
{
	int i;
	u32 reg;

	arm_init_before_mmu();
	/* Set up an identity-mapping for all 4GB, rw for everyone */
	for (i = 0; i < 4096; i++)
		page_table[i] = i << 20 | (3 << 10) | 0x12;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		dram_bank_mmu_setup(i);
	}

	/* Copy the page table address to cp15 */
	asm volatile("mcr p15, 0, %0, c2, c0, 0"
		     : : "r" (page_table) : "memory");
	/* Set the access control to all-supervisor */
	asm volatile("mcr p15, 0, %0, c3, c0, 0"
		     : : "r" (~0));
	/* and enable the mmu */
	reg = get_cr();	/* get control reg. */
	cp_delay();
	set_cr(reg | CR_M);
}

static int mmu_enabled(void)
{
	return get_cr() & CR_M;
}

/* cache_bit must be either CR_I or CR_C */
static void cache_enable(uint32_t cache_bit)
{
	uint32_t reg;

	/* The data cache is not active unless the mmu is enabled too */
	if ((cache_bit == CR_C) && !mmu_enabled())
		mmu_setup();
	reg = get_cr();	/* get control reg. */
	cp_delay();
	set_cr(reg | cache_bit);
}


void enable_caches(void)
{
	cache_enable(CR_I);
	cache_enable(CR_C);
}

void invalidate_dcache_all(void)
{
	v7_maint_dcache_all(ARMV7_DCACHE_INVAL_ALL);

	v7_outer_cache_inval_all();
}

/*
 * Performs a clean & invalidation of the entire data cache
 * at all levels
 */
void flush_dcache_all(void)
{
	v7_maint_dcache_all(ARMV7_DCACHE_CLEAN_INVAL_ALL);

	v7_outer_cache_flush_all();
}

/* cache_bit must be either CR_I or CR_C */
static void cache_disable(uint32_t cache_bit)
{
	uint32_t reg;

	reg = get_cr();
	cp_delay();

	if (cache_bit == CR_C) {
		/* if cache isn;t enabled no need to disable */
		if ((reg & CR_C) != CR_C)
			return;
		/* if disabling data cache, disable mmu too */
		cache_bit |= CR_M;
		flush_dcache_all();
	}
	set_cr(reg & ~cache_bit);
}

void icache_disable(void)
{
	cache_disable(CR_I);
}

void dcache_disable(void)
{
	cache_disable(CR_C);
}

int disable_interrupts (void)
{
	unsigned long old,temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (temp)
			     :
			     : "memory");
	return (old & 0x80) == 0;
}

/* Invalidate entire I-cache and branch predictor array */
void invalidate_icache_all(void)
{
	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm volatile ("mcr p15, 0, %0, c7, c5, 0" : : "r" (0));

	/* Invalidate entire branch predictor array */
	asm volatile ("mcr p15, 0, %0, c7, c5, 6" : : "r" (0));

	/* Full system DSB - make sure that the invalidation is complete */
	CP15DSB;

	/* ISB - make sure the instruction stream sees it */
	CP15ISB;
}


int cleanup_before_linux(void)
{
	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */
	disable_interrupts();

	/*
	 * Turn off I-cache and invalidate it
	 */
	icache_disable();
	invalidate_icache_all();

	/*
	 * turn off D-cache
	 * dcache_disable() in turn flushes the d-cache and disables MMU
	 */
	dcache_disable();
	v7_outer_cache_disable();

	/*
	 * After D-cache is flushed and before it is disabled there may
	 * be some new valid entries brought into the cache. We are sure
	 * that these lines are not dirty and will not affect our execution.
	 * (because unwinding the call-stack and setting a bit in CP15 SCTRL
	 * is all we did during this. We have not pushed anything on to the
	 * stack. Neither have we affected any static data)
	 * So just invalidate the entire d-cache again to avoid coherency
	 * problems for kernel
	 */
	invalidate_dcache_all();

	clear_l2cache_err();

	/* Yes. I am a paranoid */
	/* Full system DSB - make sure that the invalidation is complete */
	CP15DSB;
	/* ISB - make sure the instruction stream sees it */
	CP15ISB;

	return 0;
}
