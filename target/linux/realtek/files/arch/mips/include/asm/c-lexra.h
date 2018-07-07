/*
 *  Lexra specific mmu/cache definitions
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */
#ifndef __ASM_C_LEXRA_H
#define __ASM_C_LEXRA_H

/*
 * Lexra I-MEM/D-MEM
 */
#define __IRAM			__attribute__ ((section(".iram")))
#define __DRAM			__attribute__ ((section(".dram")))

#define __NOMIPS16		__attribute__ ((nomips16))
#define __MIPS16		__attribute__ ((mips16))


extern unsigned long lexra_icache_size;
extern unsigned long lexra_dcache_size;
extern unsigned long lexra_icache_lsize;
extern unsigned long lexra_dcache_lsize;

extern int lexra_has_dcacheop;
extern int lexra_has_wb_dcache;

extern int lexra_imem0_size;
extern int lexra_imem1_size;
extern int lexra_has_dual_cmem;

extern void plat_lexra_cache_init(void);

#endif /* __ASM_C_LEXRA_H */
