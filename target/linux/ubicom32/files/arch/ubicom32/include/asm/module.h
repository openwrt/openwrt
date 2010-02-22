/*
 * arch/ubicom32/include/asm/module.h
 *   Ubicom32 architecture specific module definitions.
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
#ifndef _ASM_UBICOM32_MODULE_H
#define _ASM_UBICOM32_MODULE_H

struct mod_arch_specific {
	void *ocm_inst;
	int ocm_inst_size;
};

#define Elf_Shdr Elf32_Shdr
#define Elf_Sym Elf32_Sym
#define Elf_Ehdr Elf32_Ehdr

#define ARCH_PROC_MODULES_EXTRA(m,mod) \
	seq_printf(m, " OCM(%d bytes @ 0x%p)", \
		   (mod)->arch.ocm_inst_size, (mod)->arch.ocm_inst)

#define ARCH_OOPS_MODULE_EXTRA(mod) \
	printk(KERN_INFO "%p %u OCM(%p %u)\n", \
		(mod)->module_core, (mod)->core_size, \
		(mod)->arch.ocm_inst, (mod)->arch.ocm_inst_size)
#endif /* _ASM_UBICOM32_MODULE_H */
