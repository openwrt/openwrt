/*
 * arch/ubicom32/kernel/asm-offsets.c
 *   Ubicom32 architecture definitions needed by assembly language modules.
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
/*
 * This program is used to generate definitions needed by
 * assembly language modules.
 *
 * We use the technique used in the OSF Mach kernel code:
 * generate asm statements containing #defines,
 * compile this file to assembler, and then extract the
 * #defines from the assembly-language output.
 */

#include <linux/module.h>
#include <linux/stddef.h>
#include <linux/sched.h>
#include <linux/kernel_stat.h>
#include <linux/ptrace.h>
#include <linux/hardirq.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/thread_info.h>

#define DEFINE(sym, val) \
        asm volatile("\n->" #sym " %0 " #val : : "i" (val))

#define BLANK() asm volatile("\n->" : : )

int main(void)
{
	/* offsets into the task struct */
	DEFINE(TASK_STATE, offsetof(struct task_struct, state));
	DEFINE(TASK_FLAGS, offsetof(struct task_struct, flags));
	DEFINE(TASK_PTRACE, offsetof(struct task_struct, ptrace));
	DEFINE(TASK_BLOCKED, offsetof(struct task_struct, blocked));
	DEFINE(TASK_THREAD, offsetof(struct task_struct, thread));
	DEFINE(TASK_THREAD_INFO, offsetof(struct task_struct, stack));
	DEFINE(TASK_MM, offsetof(struct task_struct, mm));
	DEFINE(TASK_ACTIVE_MM, offsetof(struct task_struct, active_mm));

	/* offsets into the kernel_stat struct */
//	DEFINE(STAT_IRQ, offsetof(struct kernel_stat, irqs));

	/* offsets into the irq_cpustat_t struct */
	DEFINE(CPUSTAT_SOFTIRQ_PENDING, offsetof(irq_cpustat_t, __softirq_pending));

	/* offsets into the thread struct */
	DEFINE(THREAD_D10, offsetof(struct thread_struct, d10));
	DEFINE(THREAD_D11, offsetof(struct thread_struct, d11));
	DEFINE(THREAD_D12, offsetof(struct thread_struct, d12));
	DEFINE(THREAD_D13, offsetof(struct thread_struct, d13));
	DEFINE(THREAD_A1, offsetof(struct thread_struct, a1));
	DEFINE(THREAD_A2, offsetof(struct thread_struct, a2));
	DEFINE(THREAD_A5, offsetof(struct thread_struct, a5));
	DEFINE(THREAD_A6, offsetof(struct thread_struct, a6));
	DEFINE(THREAD_SP, offsetof(struct thread_struct, sp));

	/* offsets into the pt_regs */
	DEFINE(PT_D0, offsetof(struct pt_regs, dn[0]));
	DEFINE(PT_D1, offsetof(struct pt_regs, dn[1]));
	DEFINE(PT_D2, offsetof(struct pt_regs, dn[2]));
	DEFINE(PT_D3, offsetof(struct pt_regs, dn[3]));
	DEFINE(PT_D4, offsetof(struct pt_regs, dn[4]));
	DEFINE(PT_D5, offsetof(struct pt_regs, dn[5]));
	DEFINE(PT_D6, offsetof(struct pt_regs, dn[6]));
	DEFINE(PT_D7, offsetof(struct pt_regs, dn[7]));
	DEFINE(PT_D8, offsetof(struct pt_regs, dn[8]));
	DEFINE(PT_D9, offsetof(struct pt_regs, dn[9]));
	DEFINE(PT_D10, offsetof(struct pt_regs, dn[10]));
	DEFINE(PT_D11, offsetof(struct pt_regs, dn[11]));
	DEFINE(PT_D12, offsetof(struct pt_regs, dn[12]));
	DEFINE(PT_D13, offsetof(struct pt_regs, dn[13]));
	DEFINE(PT_D14, offsetof(struct pt_regs, dn[14]));
	DEFINE(PT_D15, offsetof(struct pt_regs, dn[15]));
	DEFINE(PT_A0, offsetof(struct pt_regs, an[0]));
	DEFINE(PT_A1, offsetof(struct pt_regs, an[1]));
	DEFINE(PT_A2, offsetof(struct pt_regs, an[2]));
	DEFINE(PT_A3, offsetof(struct pt_regs, an[3]));
	DEFINE(PT_A4, offsetof(struct pt_regs, an[4]));
	DEFINE(PT_A5, offsetof(struct pt_regs, an[5]));
	DEFINE(PT_A6, offsetof(struct pt_regs, an[6]));
	DEFINE(PT_A7, offsetof(struct pt_regs, an[7]));
	DEFINE(PT_SP, offsetof(struct pt_regs, an[7]));

	DEFINE(PT_ACC0HI, offsetof(struct pt_regs, acc0[0]));
	DEFINE(PT_ACC0LO, offsetof(struct pt_regs, acc0[1]));
	DEFINE(PT_MAC_RC16, offsetof(struct pt_regs, mac_rc16));

	DEFINE(PT_ACC1HI, offsetof(struct pt_regs, acc1[0]));
	DEFINE(PT_ACC1LO, offsetof(struct pt_regs, acc1[1]));

	DEFINE(PT_SOURCE3, offsetof(struct pt_regs, source3));
	DEFINE(PT_INST_CNT, offsetof(struct pt_regs, inst_cnt));
	DEFINE(PT_CSR, offsetof(struct pt_regs, csr));
	DEFINE(PT_DUMMY_UNUSED, offsetof(struct pt_regs, dummy_unused));

	DEFINE(PT_INT_MASK0, offsetof(struct pt_regs, int_mask0));
	DEFINE(PT_INT_MASK1, offsetof(struct pt_regs, int_mask1));

	DEFINE(PT_PC, offsetof(struct pt_regs, pc));

	DEFINE(PT_TRAP_CAUSE, offsetof(struct pt_regs, trap_cause));

	DEFINE(PT_SIZE, sizeof(struct pt_regs));

	DEFINE(PT_FRAME_TYPE, offsetof(struct pt_regs, frame_type));

	DEFINE(PT_ORIGINAL_D0, offsetof(struct pt_regs, original_dn_0));
	DEFINE(PT_PREVIOUS_PC, offsetof(struct pt_regs, previous_pc));

	/* offsets into the kernel_stat struct */
//	DEFINE(STAT_IRQ, offsetof(struct kernel_stat, irqs));

	/* signal defines */
	DEFINE(SIGSEGV, SIGSEGV);
	//DEFINE(SEGV_MAPERR, SEGV_MAPERR);
	DEFINE(SIGTRAP, SIGTRAP);
	//DEFINE(TRAP_TRACE, TRAP_TRACE);

	DEFINE(PT_PTRACED, PT_PTRACED);
	DEFINE(PT_DTRACE, PT_DTRACE);

	DEFINE(ASM_THREAD_SIZE, THREAD_SIZE);

	/* Offsets in thread_info structure */
	DEFINE(TI_TASK, offsetof(struct thread_info, task));
	DEFINE(TI_EXECDOMAIN, offsetof(struct thread_info, exec_domain));
	DEFINE(TI_FLAGS, offsetof(struct thread_info, flags));
	DEFINE(TI_PREEMPTCOUNT, offsetof(struct thread_info, preempt_count));
	DEFINE(TI_CPU, offsetof(struct thread_info, cpu));
	DEFINE(TI_INTR_NESTING, offsetof(struct thread_info, interrupt_nesting));
	DEFINE(ASM_TIF_NEED_RESCHED, TIF_NEED_RESCHED);
	DEFINE(ASM_TIF_SYSCALL_TRACE, TIF_SYSCALL_TRACE);
	DEFINE(ASM_TIF_SIGPENDING, TIF_SIGPENDING);

	return 0;
}
