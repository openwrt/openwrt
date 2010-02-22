/*
 * arch/ubicom32/include/asm/elf.h
 *   Definitions for elf executable format for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_ELF_H
#define _ASM_UBICOM32_ELF_H

/*
 * ELF register definitions..
 */

#include <asm/ptrace.h>
#include <asm/user.h>

/*
 * Processor specific flags for the ELF header e_flags field.
 */
#define EF_UBICOM32_V3		0x00000001	/* -fmarch=ubicom32v3 */
#define EF_UBICOM32_V4		0x00000002	/* -fmarch=ubicom32v4 */
#define EF_UBICOM32_PIC		0x80000000	/* -fpic */
#define EF_UBICOM32_FDPIC	0x40000000	/* -mfdpic */

/*
 * Ubicom32 ELF relocation types
 */
#define R_UBICOM32_NONE			0
#define R_UBICOM32_16			1
#define R_UBICOM32_32			2
#define R_UBICOM32_LO16			3
#define R_UBICOM32_HI16			4
#define R_UBICOM32_21_PCREL		5
#define R_UBICOM32_24_PCREL		6
#define R_UBICOM32_HI24			7
#define R_UBICOM32_LO7_S		8
#define R_UBICOM32_LO7_2_S		9
#define R_UBICOM32_LO7_4_S		10
#define R_UBICOM32_LO7_D		11
#define R_UBICOM32_LO7_2_D		12
#define R_UBICOM32_LO7_4_D		13
#define R_UBICOM32_32_HARVARD		14
#define R_UBICOM32_LO7_CALLI		15
#define R_UBICOM32_LO16_CALLI		16
#define R_UBICOM32_GOT_HI24		17
#define R_UBICOM32_GOT_LO7_S		18
#define R_UBICOM32_GOT_LO7_2_S		19
#define R_UBICOM32_GOT_LO7_4_S		20
#define R_UBICOM32_GOT_LO7_D		21
#define R_UBICOM32_GOT_LO7_2_D		22
#define R_UBICOM32_GOT_LO7_4_D		23
#define R_UBICOM32_FUNCDESC_GOT_HI24    24
#define R_UBICOM32_FUNCDESC_GOT_LO7_S   25
#define R_UBICOM32_FUNCDESC_GOT_LO7_2_S 26
#define R_UBICOM32_FUNCDESC_GOT_LO7_4_S 27
#define R_UBICOM32_FUNCDESC_GOT_LO7_D   28
#define R_UBICOM32_FUNCDESC_GOT_LO7_2_D 29
#define R_UBICOM32_FUNCDESC_GOT_LO7_4_D 30
#define R_UBICOM32_GOT_LO7_CALLI        31
#define R_UBICOM32_FUNCDESC_GOT_LO7_CALLI 32
#define R_UBICOM32_FUNCDESC_VALUE       33
#define R_UBICOM32_FUNCDESC             34
#define R_UBICOM32_GOTOFFSET_LO         35
#define R_UBICOM32_GOTOFFSET_HI         36
#define R_UBICOM32_FUNCDESC_GOTOFFSET_LO 37
#define R_UBICOM32_FUNCDESC_GOTOFFSET_HI 38
#define R_UBICOM32_GNU_VTINHERIT        200
#define R_UBICOM32_GNU_VTENTRY          201

typedef unsigned long elf_greg_t;

#define ELF_NGREG (sizeof(struct pt_regs) / sizeof(elf_greg_t))
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef struct user_ubicom32fp_struct elf_fpregset_t;

/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch(x) ((x)->e_machine == EM_UBICOM32)

#define elf_check_fdpic(x) ((x)->e_flags & EF_UBICOM32_FDPIC)

#define elf_check_const_displacement(x) ((x)->e_flags & EF_UBICOM32_PIC)

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS32
#define ELF_DATA	ELFDATA2MSB
#define ELF_ARCH	EM_UBICOM32

/* For SVR4/m68k the function pointer to be registered with `atexit' is
   passed in %a1.  Although my copy of the ABI has no such statement, it
   is actually used on ASV.  */
#define ELF_PLAT_INIT(_r, load_addr)	_r->a1 = 0

#define ELF_FDPIC_PLAT_INIT(_regs, _exec_map_addr, _interp_map_addr,	\
			    _dynamic_addr)				\
	do {								\
		_regs->dn[1]	= _exec_map_addr;			\
		_regs->dn[2]	= _interp_map_addr;			\
		_regs->dn[3]	= _dynamic_addr;			\
		_regs->an[1]	= 0; /* dl_fini will be set by ldso */	\
	} while (0)

#define USE_ELF_CORE_DUMP
#define ELF_EXEC_PAGESIZE	4096

#ifdef __KERNEL__
#ifdef CONFIG_UBICOM32_V4
#define ELF_FDPIC_CORE_EFLAGS	(EF_UBICOM32_FDPIC | EF_UBICOM32_V4)
#elif defined CONFIG_UBICOM32_V3
#define ELF_FDPIC_CORE_EFLAGS	(EF_UBICOM32_FDPIC | EF_UBICOM32_V3)
#else
#error Unknown/Unsupported ubicom32 architecture.
#endif
#endif

/* This is the location that an ET_DYN program is loaded if exec'ed.  Typical
   use of this is to invoke "./ld.so someprog" to test out a new version of
   the loader.  We need to make sure that it is out of the way of the program
   that it will "exec", and that there is sufficient room for the brk.  */

#define ELF_ET_DYN_BASE         0xD0000000UL

/*
 * For Ubicom32, the elf_gregset_t and struct pt_regs are the same size
 * data structure so a copy is performed instead of providing the
 * ELF_CORE_COPY_REGS macro.
 */

/*
 * ELF_CORE_COPY_TASK_REGS is needed to dump register state from multi threaded user projects.
 */
extern int dump_task_regs(struct task_struct *, elf_gregset_t *);
#define ELF_CORE_COPY_TASK_REGS(tsk, elf_regs) dump_task_regs(tsk, elf_regs)

/* This yields a mask that user programs can use to figure out what
   instruction set this cpu supports.  */

#define ELF_HWCAP	(0)

/* This yields a string that ld.so will use to load implementation
   specific libraries for optimization.  This is more specific in
   intent than poking at uname or /proc/cpuinfo.  */

#define ELF_PLATFORM  (NULL)

#define SET_PERSONALITY(ex, ibcs2) set_personality((ibcs2)?PER_SVR4:PER_LINUX)

#endif /* _ASM_UBICOM32_ELF_H */
