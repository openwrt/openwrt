/*
 * arch/ubicom32/include/asm/ptrace.h
 *   Ubicom32 architecture ptrace support.
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
#ifndef _ASM_UBICOM32_PTRACE_H
#define _ASM_UBICOM32_PTRACE_H

#ifndef __ASSEMBLY__

/*
 * We use hard coded constants because this is shared with user
 * space and the values are NOT allowed to change.  Only fields
 * that are intended to be exposed get values.
 */
#define PT_D0           0
#define PT_D1           4
#define PT_D2           8
#define PT_D3           12
#define PT_D4           16
#define PT_D5           20
#define PT_D6           24
#define PT_D7           28
#define PT_D8           32
#define PT_D9           36
#define PT_D10          40
#define PT_D11          44
#define PT_D12          48
#define PT_D13          52
#define PT_D14          56
#define PT_D15          60
#define PT_A0           64
#define PT_A1           68
#define PT_A2           72
#define PT_A3           76
#define PT_A4           80
#define PT_A5           84
#define PT_A6           88
#define PT_A7           92
#define PT_SP           92
#define PT_ACC0HI       96
#define PT_ACC0LO       100
#define PT_MAC_RC16     104
#define PT_ACC1HI       108
#define PT_ACC1LO       112
#define PT_SOURCE3      116
#define PT_INST_CNT     120
#define PT_CSR          124
#define PT_DUMMY_UNUSED 128
#define PT_INT_MASK0    132
#define PT_INT_MASK1    136
#define PT_TRAP_CAUSE   140
#define PT_PC           144
#define PT_ORIGINAL_D0  148
#define PT_FRAME_TYPE   152

/*
 * The following 'registers' are not registers at all but are used
 * locate the relocated sections.
 */
#define PT_TEXT_ADDR		200
#define PT_TEXT_END_ADDR	204
#define PT_DATA_ADDR		208
#define PT_EXEC_FDPIC_LOADMAP	212
#define PT_INTERP_FDPIC_LOADMAP	216

/*
 * This struct defines the way the registers are stored on the
 * stack during a system call.
 */
enum thread_type {
	NORMAL_THREAD,
	KERNEL_THREAD,
};

#define UBICOM32_FRAME_TYPE_SYSCALL	-1 /* System call frame */
#define UBICOM32_FRAME_TYPE_INVALID	0 /* Invalid frame, no longer in use */
#define UBICOM32_FRAME_TYPE_INTERRUPT	1 /* Interrupt frame */
#define UBICOM32_FRAME_TYPE_TRAP	2 /* Trap frame */
#define UBICOM32_FRAME_TYPE_SIGTRAMP	3 /* Signal trampoline frame. */
#define UBICOM32_FRAME_TYPE_NEW_THREAD	4 /* New Thread. */

struct pt_regs {
	/*
	 * Data Registers
	 */
	unsigned long dn[16];

	/*
	 * Address Registers
	 */
	unsigned long an[8];

	/*
	 * Per thread misc registers.
	 */
	unsigned long acc0[2];
	unsigned long mac_rc16;
	unsigned long acc1[2];
	unsigned long source3;
	unsigned long inst_cnt;
	unsigned long csr;
	unsigned long dummy_unused;
	unsigned long int_mask0;
	unsigned long int_mask1;
	unsigned long trap_cause;
	unsigned long pc;
	unsigned long original_dn_0;

	/*
	 * Frame type. Syscall frames are -1. For other types look above.
	 */
	unsigned long frame_type;

	/*
	 * These fields are not exposed to ptrace.
	 */
	unsigned long previous_pc;
	long nesting_level;		/* When the kernel in in user space this
					 * will be -1. */
	unsigned long thread_type;	/* This indicates if this is a kernel
					 * thread. */
};

/*
 * This is the extended stack used by signal handlers and the context
 * switcher: it's pushed after the normal "struct pt_regs".
 */
struct switch_stack {
	unsigned long  dummy;
};

#ifdef __KERNEL__

/* Arbitrarily choose the same ptrace numbers as used by the Sparc code. */
#define PTRACE_GETREGS		12
#define PTRACE_SETREGS		13

#ifndef PS_S
#define PS_S  (0x2000)
#define PS_M  (0x1000)
#endif

extern  int __user_mode(unsigned long sp);

#define user_mode(regs) (__user_mode((regs->an[7])))
#define user_stack(regs) ((regs)->an[7])
#define instruction_pointer(regs) ((regs)->pc)
#define profile_pc(regs) instruction_pointer(regs)
extern void show_regs(struct pt_regs *);
#endif /* __KERNEL__ */

#endif /* __ASSEMBLY__ */

#endif /* _ASM_UBICOM32_PTRACE_H */
