/*
 * arch/ubicom32/kernel/process.c
 *   Ubicom32 architecture-dependent process handling.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 1995  Hamish Macdonald
 *
 * 68060 fixes by Jesper Skov
 *
 * uClinux changes
 * Copyright (C) 2000-2002, David McCullough <davidm@snapgear.com>
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
 * This file handles the architecture-dependent parts of process handling..
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/user.h>
#include <linux/a.out.h>
#include <linux/interrupt.h>
#include <linux/reboot.h>
#include <linux/fs.h>
#include <linux/pm.h>

#include <linux/uaccess.h>
#include <asm/system.h>
#include <asm/traps.h>
#include <asm/machdep.h>
#include <asm/setup.h>
#include <asm/pgtable.h>
#include <asm/ip5000.h>
#include <asm/range-protect.h>

#define DUMP_RANGE_REGISTER(REG, IDX) asm volatile ( \
        "       move.4          %0, "REG"_RANGE"IDX"_EN \n\t" \
        "       move.4          %1, "REG"_RANGE"IDX"_LO \n\t" \
        "       move.4          %2, "REG"_RANGE"IDX"_HI \n\t" \
                : "=d"(en), "=d"(lo), "=d"(hi) \
        ); \
        printk(KERN_NOTICE REG"Range"IDX": en:%08x, range: %08x-%08x\n", \
                (unsigned int)en, \
                (unsigned int)lo, \
                (unsigned int)hi)

asmlinkage void ret_from_fork(void);

void (*pm_power_off)(void) = machine_power_off;
EXPORT_SYMBOL(pm_power_off);

/* machine-dependent / hardware-specific power functions */
void (*mach_reset)(void);
void (*mach_halt)(void);
void (*mach_power_off)(void);

/*
 * cpu_idle()
 *	The idle thread.
 *
 * Our idle loop suspends and is woken up by a timer interrupt.
 */
void cpu_idle(void)
{
	while (1) {
		local_irq_disable();
		while (!need_resched()) {
			local_irq_enable();
			thread_suspend();
			local_irq_disable();
		}
		local_irq_enable();
		preempt_enable_no_resched();
		schedule();
		preempt_disable();
	}
}

/*
 * dump_fpu()
 *
 *	Fill in the fpu structure for a core dump. (just a stub as we don't have
 *	an fpu)
 */
int dump_fpu(struct pt_regs *regs, elf_fpregset_t * fpregs)
{
	return 1;
}

/*
 * machine_restart()
 *	Resets the system.
 */
void machine_restart(char *__unused)
{
	/*
	 * Disable all threads except myself. We can do this
	 * directly without needing to call smp_send_stop
	 * because we have a unique architecture where
	 * one thread can disable one or more other threads.
	 */
	thread_disable_others();

	/*
	 * Call the hardware-specific machine reset function.
	 */
	if (mach_reset) {
		mach_reset();
	}

	printk(KERN_EMERG "System Restarting\n");

	/*
	 * Set watchdog to trigger (after 1ms delay) (12 Mhz is the fixed OSC)
	 */
	UBICOM32_IO_TIMER->tkey = TIMER_TKEYVAL;
	UBICOM32_IO_TIMER->wdcom = UBICOM32_IO_TIMER->mptval +
		(12000000 / 1000);
	UBICOM32_IO_TIMER->wdcfg = 0;
	UBICOM32_IO_TIMER->tkey = 0;

	/*
	 * Wait for watchdog
	 */
	asm volatile (
		"	move.4		MT_EN, #0		\n\t"
		"	pipe_flush	0			\n\t"
	);

	local_irq_disable();
	for (;;) {
		thread_suspend();
	}
}

/*
 * machine_halt()
 *	Halt the machine.
 *
 * Similar to machine_power_off, but don't shut off power.  Add code
 * here to freeze the system for e.g. post-mortem debug purpose when
 * possible.  This halt has nothing to do with the idle halt.
 */
void machine_halt(void)
{
	/*
	 * Disable all threads except myself. We can do this
	 * directly without needing to call smp_send_stop
	 * because we have a unique architecture where
	 * one thread can disable one or more other threads.
	 */
	thread_disable_others();

	/*
	 * Call the hardware-specific machine halt function.
	 */
	if (mach_halt) {
		mach_halt();
	}

	printk(KERN_EMERG "System Halted, OK to turn off power\n");
	local_irq_disable();
	for (;;) {
		thread_suspend();
	}
}

/*
 * machine_power_off()
 *	Turn the power off, if a power off handler is defined, otherwise, spin
 *	endlessly.
 */
void machine_power_off(void)
{
	/*
	 * Disable all threads except myself. We can do this
	 * directly without needing to call smp_send_stop
	 * because we have a unique architecture where
	 * one thread can disable one or more other threads.
	 */
	thread_disable_others();

	/*
	 * Call the hardware-specific machine power off function.
	 */
	if (mach_power_off) {
		mach_power_off();
	}

	printk(KERN_EMERG "System Halted, OK to turn off power\n");
	local_irq_disable();
	for (;;) {
		thread_suspend();
	}
}

/*
 * address_is_valid()
 * 	check if an address is valid -- (for read access)
 */
static bool address_is_valid(const void *address)
{
	int addr = (int)address;
	unsigned long socm, eocm, sdram, edram;

	if (addr & 3)
		return false;

	processor_ocm(&socm, &eocm);
	processor_dram(&sdram, &edram);
	if (addr >= socm && addr < eocm)
		return true;

	if (addr >= sdram && addr < edram)
		return true;

	return false;
}

/*
 * vma_path_name_is_valid()
 * 	check if path_name of a vma is a valid string
 */
static bool vma_path_name_is_valid(const char *str)
{
#define MAX_NAME_LEN 256
	int i = 0;
	if (!address_is_valid(str))
		return false;

	for (; i < MAX_NAME_LEN; i++, str++) {
		if (*str == '\0')
			return true;
	}

	return false;
}

/*
 * show_vmas()
 * 	show vma info of a process
 */
void show_vmas(struct task_struct *task)
{
#ifdef CONFIG_DEBUG_VERBOSE
#define UBICOM32_MAX_VMA_COUNT 1024

	struct vm_area_struct *vma;
	struct file *file;
	char *name = "";
	int flags, loop = 0;

	printk(KERN_NOTICE "Start of vma list\n");

	if (!address_is_valid(task) || !address_is_valid(task->mm))
		goto error;

	vma = task->mm->mmap;
	while (vma) {
		if (!address_is_valid(vma))
			goto error;

		flags = vma->vm_flags;
		file = vma->vm_file;

		if (file) {
			/* seems better to use dentry op here, but sanity check is easier this way */
			if (!address_is_valid(file) || !address_is_valid(file->f_path.dentry) || !vma_path_name_is_valid(file->f_path.dentry->d_name.name))
				goto error;

			name = (char *)file->f_path.dentry->d_name.name;
		}

		/* Similar to /proc/pid/maps format */
		printk(KERN_NOTICE "%08lx-%08lx %c%c%c%c %08lx %s\n",
			vma->vm_start,
			vma->vm_end,
			flags & VM_READ ? 'r' : '-',
			flags & VM_WRITE ? 'w' : '-',
			flags & VM_EXEC ? 'x' : '-',
			flags & VM_MAYSHARE ? flags & VM_SHARED ? 'S' : 's' : 'p',
			vma->vm_pgoff << PAGE_SHIFT,
			name);

		vma = vma->vm_next;

		if (loop++ > UBICOM32_MAX_VMA_COUNT)
			goto error;
	}

	printk(KERN_NOTICE "End of vma list\n");
	return;

error:
	printk(KERN_NOTICE "\nCorrupted vma list, abort!\n");
#endif
}

/*
 * show_regs()
 *	Print out all of the registers.
 */
void show_regs(struct pt_regs *regs)
{
	unsigned int i;
	unsigned int en, lo, hi;

	printk(KERN_NOTICE "regs: %p, tid: %d\n",
		(void *)regs,
		thread_get_self());

	printk(KERN_NOTICE "pc: %08x, previous_pc: %08x\n\n",
		(unsigned int)regs->pc,
		(unsigned int)regs->previous_pc);

	printk(KERN_NOTICE "Data registers\n");
	for (i = 0; i < 16; i++) {
		printk("D%02d: %08x, ", i, (unsigned int)regs->dn[i]);
		if ((i % 4) == 3) {
			printk("\n");
		}
	}
	printk("\n");

	printk(KERN_NOTICE "Address registers\n");
	for (i = 0; i < 8; i++) {
		printk("A%02d: %08x, ", i, (unsigned int)regs->an[i]);
		if ((i % 4) == 3) {
			printk("\n");
		}
	}
	printk("\n");

	printk(KERN_NOTICE "acc0: %08x-%08x, acc1: %08x-%08x\n",
		(unsigned int)regs->acc0[1],
		(unsigned int)regs->acc0[0],
		(unsigned int)regs->acc1[1],
		(unsigned int)regs->acc1[0]);

	printk(KERN_NOTICE "mac_rc16: %08x, source3: %08x\n",
		(unsigned int)regs->mac_rc16,
		(unsigned int)regs->source3);

	printk(KERN_NOTICE "inst_cnt: %08x, csr: %08x\n",
		(unsigned int)regs->inst_cnt,
		(unsigned int)regs->csr);

	printk(KERN_NOTICE "int_mask0: %08x, int_mask1: %08x\n",
		(unsigned int)regs->int_mask0,
		(unsigned int)regs->int_mask1);

	/*
	 * Dump range registers
	 */
	DUMP_RANGE_REGISTER("I", "0");
	DUMP_RANGE_REGISTER("I", "1");
	DUMP_RANGE_REGISTER("I", "2");
	DUMP_RANGE_REGISTER("I", "3");
	DUMP_RANGE_REGISTER("D", "0");
	DUMP_RANGE_REGISTER("D", "1");
	DUMP_RANGE_REGISTER("D", "2");
	DUMP_RANGE_REGISTER("D", "3");
	DUMP_RANGE_REGISTER("D", "4");

	printk(KERN_NOTICE "frame_type: %d, nesting_level: %d, thread_type %d\n\n",
		(int)regs->frame_type,
		(int)regs->nesting_level,
		(int)regs->thread_type);
}

/*
 * kernel_thread_helper()
 *	On execution d0 will be 0, d1 will be the argument to be passed to the
 *	kernel function.  d2 contains the kernel function that needs to get
 *	called. d3 will contain address to do_exit which need to get moved
 *	into a5. On return from fork the child thread d0 will be 0. We call
 *	this dummy function which in turn loads the argument
 */
asmlinkage void kernel_thread_helper(void);

/*
 * kernel_thread()
 *	Create a kernel thread
 */
int kernel_thread(int (*fn)(void *), void *arg, unsigned long flags)
{
	struct pt_regs regs;

	memset(&regs, 0, sizeof(regs));

	regs.dn[1] = (unsigned long)arg;
	regs.dn[2] = (unsigned long)fn;
	regs.dn[3] = (unsigned long)do_exit;
	regs.an[5] = (unsigned long)kernel_thread_helper;
	regs.pc = (unsigned long)kernel_thread_helper;
	regs.nesting_level = 0;
	regs.thread_type = KERNEL_THREAD;

	return do_fork(flags | CLONE_VM | CLONE_UNTRACED,
		       0, &regs, 0, NULL, NULL);
}
EXPORT_SYMBOL(kernel_thread);

/*
 * flush_thread()
 *	XXX todo
 */
void flush_thread(void)
{
	/* XXX todo */
}

/*
 * sys_fork()
 *	Not implemented on no-mmu.
 */
asmlinkage int sys_fork(struct pt_regs *regs)
{
	/* fork almost works, enough to trick you into looking elsewhere :-( */
	return -EINVAL;
}

/*
 * sys_vfork()
 *	By the time we get here, the non-volatile registers have also been saved
 *	on the stack. We do some ugly pointer stuff here.. (see also copy_thread
 *	which does context copy).
 */
asmlinkage int sys_vfork(struct pt_regs *regs)
{
	unsigned long old_sp = regs->an[7];
	unsigned long old_a5 = regs->an[5];
	unsigned long old_return_address;
	long do_fork_return;

	/*
	 * Read the old retrun address from the stack.
	 */
	if (copy_from_user(&old_return_address,
			   (void *)old_sp, sizeof(unsigned long))) {
		force_sig(SIGSEGV, current);
		return 0;
	}

	/*
	 * Pop the vfork call frame by setting a5 and pc to the old_return
	 * address and incrementing the stack pointer by 4.
	 */
	regs->an[5] = old_return_address;
	regs->pc = old_return_address;
	regs->an[7] += 4;

	do_fork_return = do_fork(CLONE_VFORK | CLONE_VM | SIGCHLD,
				 regs->an[7], regs, 0, NULL, NULL);

	/*
	 * Now we have to test if the return code is an error. If it is an error
	 * then restore the frame and we will execute error processing in user
	 * space. Other wise the child and the parent will return to the correct
	 * places.
	 */
	if ((unsigned long)(do_fork_return) >= (unsigned long)(-125)) {
		/*
		 * Error case. We need to restore the frame.
		 */
		regs->an[5] = old_a5;
		regs->pc = old_a5;
		regs->an[7] = old_sp;
	}

	return do_fork_return;
}

/*
 * sys_clone()
 *	creates a child thread.
 */
asmlinkage int sys_clone(unsigned long clone_flags,
			 unsigned long newsp,
			 struct pt_regs *regs)
{
	if (!newsp)
		newsp = regs->an[7];
	return do_fork(clone_flags, newsp, regs, 0,
		       NULL, NULL);
}

/*
 * copy_thread()
 *	low level thread copy, only used by do_fork in kernel/fork.c
 */
int copy_thread(unsigned long clone_flags,
		unsigned long usp, unsigned long topstk,
		struct task_struct *p, struct pt_regs *regs)

{
	struct pt_regs *childregs;

	childregs = (struct pt_regs *)
		(task_stack_page(p) + THREAD_SIZE - 8) - 1;

	*childregs = *regs;

	/*
	 * Set return value for child to be 0.
	 */
	childregs->dn[0] = 0;

	if (usp)
		childregs->an[7] = usp;
	else
		childregs->an[7] = (unsigned long)task_stack_page(p) +
			THREAD_SIZE - 8;

	/*
	 * Set up the switch_to frame to return to "ret_from_fork"
	 */
	p->thread.a5 = (unsigned long)ret_from_fork;
	p->thread.sp = (unsigned long)childregs;

	return 0;
}

/*
 * sys_execve()
 *	executes a new program.
 */
asmlinkage int sys_execve(char *name, char **argv,
			  char **envp, struct pt_regs *regs)
{
	int error;
	char *filename;

	lock_kernel();
	filename = getname(name);
	error = PTR_ERR(filename);
	if (IS_ERR(filename))
		goto out;
	error = do_execve(filename, argv, envp, regs);
	putname(filename);
	asm ("       .global sys_execve_complete\n"
	     "       sys_execve_complete:");
out:
	unlock_kernel();
	return error;
}

/*
 * Return saved PC of a blocked thread.
 */
unsigned long thread_saved_pc(struct task_struct *tsk)
{
	return tsk->thread.a5;
}


unsigned long get_wchan(struct task_struct *p)
{
	unsigned long pc;

	/*
	 * If we don't have a process, or it is not the current
	 * one or not RUNNING, it makes no sense to ask for a
	 * wchan.
	 */
	if (!p || p == current || p->state == TASK_RUNNING)
		return 0;

	/*
	 * TODO: If the process is in the middle of schedule, we
	 * are supposed to do something different but for now we
	 * will return the same thing in both situations.
	 */
	pc = thread_saved_pc(p);
	if (in_sched_functions(pc))
		return pc;
	return pc;
}


/*
 * Infrequently used interface to dump task registers to core files.
 */
int dump_task_regs(struct task_struct *task, elf_gregset_t *elfregs)
{
	struct pt_regs *regs = task_pt_regs(task);
	*(struct pt_regs *)elfregs = *regs;

	return 1;
}

/*
 * __switch_to is the function that implements the contex save and
 * switch within the kernel. Since this is a function call very few
 * registers have to be saved to pull this off. d0 holds prev and we
 * want to preserve it. prev_switch is a pointer to task->thread
 * structure. This is where we will save the register state. next_switch
 * is pointer to the next task's thread structure that holds the
 * registers.
 */
asmlinkage void *__switch_to(struct task_struct *prev,
			     struct thread_struct *prev_switch,
			     struct thread_struct *next_switch)
	__attribute__((naked));
