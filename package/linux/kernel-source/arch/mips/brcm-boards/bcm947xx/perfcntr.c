/*
 * Broadcom BCM47xx Performance Counter /proc/cpuinfo support
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <asm/mipsregs.h>

/* 
 * BCM4710 performance counter register select values
 * No even-odd control-counter mapping, just counters
 */
#define PERF_DCACHE_HIT		0
#define PERF_DCACHE_MISS	1
#define PERF_ICACHE_HIT		2
#define PERF_ICACHE_MISS	3
#define PERF_ICOUNT		4

/* 
 * Move from Coprocessor 0 Register 25 Select n
 * data <- CPR[0,25,n] 
 * GPR[1] <- data
 */
#define read_bcm4710_perf_cntr(n)				\
({ int __res;							\
        __asm__ __volatile__(					\
	".set\tnoreorder\n\t"                                   \
	".set\tnoat\n\t"                                        \
	".word\t"STR(0x4001c800|(n))"\n\t"			\
	"move\t%0,$1\n\t"                                       \
	".set\tat\n\t"                                          \
	".set\treorder"                                         \
	:"=r" (__res));                                         \
	__res;})

asmlinkage unsigned int read_perf_cntr(unsigned int counter)
{
	switch (counter) {
	case PERF_DCACHE_HIT:	return read_bcm4710_perf_cntr(PERF_DCACHE_HIT);
	case PERF_DCACHE_MISS:	return read_bcm4710_perf_cntr(PERF_DCACHE_MISS);
	case PERF_ICACHE_HIT:	return read_bcm4710_perf_cntr(PERF_ICACHE_HIT);
	case PERF_ICACHE_MISS:	return read_bcm4710_perf_cntr(PERF_ICACHE_MISS);
	case PERF_ICOUNT:	return read_bcm4710_perf_cntr(PERF_ICOUNT);
	}
	return 0;
}

asmlinkage void write_perf_cntr(unsigned int counter, unsigned int val)
{
}

asmlinkage unsigned int read_perf_cntl(unsigned int counter)
{
	return 0;
}

asmlinkage void write_perf_cntl(unsigned int counter, unsigned int val)
{
}
