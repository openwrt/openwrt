/*
 * arch/ubicom32/kernel/ptrace.c
 *   Ubicom32 architecture ptrace implementation.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * (C) 1994 by Hamish Macdonald
 * Taken from linux/kernel/ptrace.c and modified for M680x0.
 * linux/kernel/ptrace.c is by Ross Biro 1/23/92, edited by Linus Torvalds
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/errno.h>
#include <linux/ptrace.h>
#include <linux/user.h>
#include <linux/signal.h>
#include <linux/uaccess.h>

#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/cacheflush.h>
#include <asm/processor.h>

/*
 * ptrace_getregs()
 *
 *	Get all user integer registers.
 */
static inline int ptrace_getregs(struct task_struct *task, void __user *uregs)
{
	struct pt_regs *regs = task_pt_regs(task);
	return copy_to_user(uregs, regs, sizeof(struct pt_regs)) ? -EFAULT : 0;
}

/*
 * ptrace_get_reg()
 *
 *	Get contents of register REGNO in task TASK.
 */
static unsigned long ptrace_get_reg(struct task_struct *task, int regno)
{
	if (regno < sizeof(struct pt_regs)) {
		struct pt_regs *pt_regs = task_pt_regs(task);
		return *(unsigned long *)((long) pt_regs + regno);
	}

	return -EIO;
}

/*
 * ptrace_put_reg()
 *	Write contents of register REGNO in task TASK.
 */
static int ptrace_put_reg(struct task_struct *task, int regno,
			  unsigned long data)
{
	if (regno <= sizeof(struct pt_regs) && regno != PT_FRAME_TYPE) {
		struct pt_regs *pt_regs = task_pt_regs(task);
		*(unsigned long *)((long) pt_regs + regno) = data;
		return 0;
	}
	return -EIO;
}

/*
 * ptrace_disable_single_step()
 *	Disable Single Step
 */
static int ptrace_disable_single_step(struct task_struct *task)
{
	/*
	 * Single Step not yet implemented, so must always be disabled
	 */
	return 0;
}

/*
 * ptrace_disable()
 *	Make sure the single step bit is not set.
 * Called by kernel/ptrace.c when detaching..
 */
void ptrace_disable(struct task_struct *child)
{
	ptrace_disable_single_step(child);
}

/*
 * arch_ptrace()
 *	architecture specific ptrace routine.
 */
long arch_ptrace(struct task_struct *child, long request, long addr, long data)
{
	int ret;
	switch (request) {
	/* when I and D space are separate, these will need to be fixed. */
	case PTRACE_PEEKTEXT: /* read word at location addr. */
	case PTRACE_PEEKDATA:
		ret = generic_ptrace_peekdata(child, addr, data);
		break;

	/* read the word at location addr in the USER area. */
	case PTRACE_PEEKUSR: {
		unsigned long tmp;

		ret = -EIO;
		if (((unsigned long) addr > PT_INTERP_FDPIC_LOADMAP)
		    || (addr & 3))
			break;

		tmp = 0;  /* Default return condition */

		ret = -EIO;
		if (addr < sizeof(struct pt_regs)) {
			tmp = ptrace_get_reg(child, addr);
		} else if (addr == PT_TEXT_ADDR) {
			tmp = child->mm->start_code;
		} else if (addr == PT_TEXT_END_ADDR) {
			tmp = child->mm->end_code;
		} else if (addr == PT_DATA_ADDR) {
			tmp = child->mm->start_data;
		} else if (addr == PT_EXEC_FDPIC_LOADMAP) {
#ifdef CONFIG_BINFMT_ELF_FDPIC
			tmp = child->mm->context.exec_fdpic_loadmap;
#endif
		} else if (addr == PT_INTERP_FDPIC_LOADMAP) {
#ifdef CONFIG_BINFMT_ELF_FDPIC
			tmp = child->mm->context.interp_fdpic_loadmap;
#endif
		} else {
			break;
		}

		ret = put_user(tmp, (unsigned long *)data);
		break;
	}

	case PTRACE_POKETEXT: /* write the word at location addr. */
	case PTRACE_POKEDATA:
		ret = generic_ptrace_pokedata(child, addr, data);

		/*
		 * If we just changed some code so we need to
		 * correct the caches
		 */
		if (request == PTRACE_POKETEXT && ret == 0) {
			flush_icache_range(addr, addr + 4);
		}
		break;

	case PTRACE_POKEUSR: /* write the word at location addr
			      * in the USER area */
		ret = -EIO;

		if (((unsigned long) addr > PT_DATA_ADDR) || (addr & 3))
			break;

		if (addr < sizeof(struct pt_regs)) {
			ret = ptrace_put_reg(child, addr, data);
		}
		break;

	case PTRACE_SYSCALL: /* continue and stop at next (return from)
			      * syscall */
	case PTRACE_CONT: { /* restart after signal. */

		ret = -EIO;
		if (!valid_signal(data))
			break;
		if (request == PTRACE_SYSCALL)
			set_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
		else
			clear_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
		child->exit_code = data;
		/* make sure the single step bit is not set. */
		ptrace_disable_single_step(child);
		wake_up_process(child);
		ret = 0;
		break;
	}

	/*
	 * make the child exit.  Best I can do is send it a sigkill.
	 * perhaps it should be put in the status that it wants to exit.
	 */
	case PTRACE_KILL: {
		ret = 0;
		if (child->exit_state == EXIT_ZOMBIE) /* already dead */
			break;
		child->exit_code = SIGKILL;
		/* make sure the single step bit is not set. */
		ptrace_disable_single_step(child);
		wake_up_process(child);
		break;
	}

	case PTRACE_DETACH:	/* detach a process that was attached. */
		ret = ptrace_detach(child, data);
		break;

	case PTRACE_GETREGS:    /* Get all gp regs from the child. */
		ptrace_getregs(child, (unsigned long *)data);
		ret = 0;
		break;

	case PTRACE_SETREGS: { /* Set all gp regs in the child. */
		int i;
		unsigned long tmp;
		int count = sizeof(struct pt_regs) / sizeof(unsigned long);
		for (i = 0; i < count; i++) {
			if (get_user(tmp, (unsigned long *) data)) {
				ret = -EFAULT;
				break;
			}
			ptrace_put_reg(child, sizeof(unsigned long) * i, tmp);
			data += sizeof(long);
		}
		ret = 0;
		break;
	}

	default:
		return ptrace_request(child, request, addr, data);
		break;
	}
	return ret;
}
/*
 * syscall_trace
 *
 * called by syscall enter/exit when the TIF_SYSCALL_TRACE bit is set.
 */
asmlinkage void syscall_trace(void)
{
	struct task_struct *cur = current;
	if (!test_thread_flag(TIF_SYSCALL_TRACE))
		return;
	if (!(cur->ptrace & PT_PTRACED))
		return;
	ptrace_notify(SIGTRAP | ((cur->ptrace & PT_TRACESYSGOOD)
				 ? 0x80 : 0));
	/*
	 * this isn't the same as continuing with a signal, but it will do
	 * for normal use.  strace only continues with a signal if the
	 * stopping signal is not SIGTRAP.  -brl
	 */
	if (cur->exit_code) {
		send_sig(cur->exit_code, current, 1);
		current->exit_code = 0;
	}
}
