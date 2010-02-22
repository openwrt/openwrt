/*
 * arch/ubicom32/kernel/traps.c
 *   Ubicom32 architecture trap handling support.
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
 * Sets up all exception vectors
 */
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/a.out.h>
#include <linux/user.h>
#include <linux/string.h>
#include <linux/linkage.h>
#include <linux/init.h>
#include <linux/ptrace.h>
#include <linux/kallsyms.h>
#include <linux/compiler.h>
#include <linux/stacktrace.h>
#include <linux/personality.h>

#include <asm/uaccess.h>
#include <asm/stacktrace.h>
#include <asm/devtree.h>
#include <asm/setup.h>
#include <asm/fpu.h>
#include <asm/system.h>
#include <asm/traps.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/machdep.h>
#include <asm/siginfo.h>
#include <asm/ip5000.h>
#include <asm/thread.h>

#define TRAP_MAX_STACK_DEPTH 20

/*
 * These symbols are filled in by the linker.
 */
extern unsigned long _stext;
extern unsigned long _etext;

extern unsigned long __ocm_text_run_begin;
extern unsigned long __data_begin;

extern void show_vmas(struct task_struct *task);

const char *trap_cause_strings[] = {
	/*0*/	"inst address decode error",
	/*1*/	"inst sync error",
	/*2*/	"inst illegal",
	/*3*/	"src1 address decode error",
	/*4*/	"dst address decode error",
	/*5*/	"src1 alignment error",
	/*6*/	"dst alignment error",
	/*7*/	"src1 sync error",
	/*8*/	"dst sync error",
	/*9*/	"DCAPT error",
	/*10*/	"inst range error",
	/*11*/	"src1 range error",
	/*12*/	"dst range error",
};

/*
 * The device tree trap node definition.
 */
struct trapnode {
	struct devtree_node dn;
	unsigned int intthread;
};

static struct trapnode *tn;;

/*
 * trap_interrupt_handler()
 *	Software Interrupt to ensure that a trap is serviced.
 */
static irqreturn_t trap_interrupt_handler(int irq, void *dummy)
{
	/* Do Nothing */
	return IRQ_HANDLED;
}

/*
 * Data used by setup_irq for the timer.
 */
static struct irqaction trap_irq = {
	.name	 = "trap",
	.flags	 = IRQF_DISABLED,
	.handler = trap_interrupt_handler,
};

/*
 * trap_cause_to_str()
 *	Convert a trap_cause into a series of printk
 */
static void trap_cause_to_str(long status)
{
	int bit;

	if ((status & ((1 << TRAP_CAUSE_TOTAL) - 1)) == 0) {
		printk(KERN_NOTICE "decode: UNKNOWN CAUSES\n");
		return;
	}

	for (bit = 0; bit < TRAP_CAUSE_TOTAL; bit++) {
		if (status & (1 << bit)) {
			printk(KERN_NOTICE "\tdecode: %08x %s\n",
			       1 << bit, trap_cause_strings[bit]);
		}
	}
}

/*
 * trap_print_information()
 *	Print the cause of the trap and additional info.
 */
static void trap_print_information(const char *str, struct pt_regs *regs)
{
	printk(KERN_WARNING "\n");

	if (current) {
		printk(KERN_WARNING "Process %s (pid: %d)\n",
			current->comm, current->pid);
	}

	if (current && current->mm) {
		printk(KERN_NOTICE "text = 0x%p-0x%p  data = 0x%p-0x%p\n"
			KERN_NOTICE "bss = 0x%p-0x%p   user-stack = 0x%p\n"
			KERN_NOTICE "\n",
			(void *)current->mm->start_code,
			(void *)current->mm->end_code,
			(void *)current->mm->start_data,
			(void *)current->mm->end_data,
			(void *)current->mm->end_data,
			(void *)current->mm->brk,
			(void *)current->mm->start_stack);
	}

	printk(KERN_WARNING "%s: Causes: 0x%08x\n", str,
			(unsigned int)regs->trap_cause);
	trap_cause_to_str(regs->trap_cause);
	show_regs(regs);
	show_stack(NULL, (unsigned long *)regs->an[7]);
	printk(KERN_NOTICE "--- End Trap --- \n");
}

/*
 * dump_stack()
 *	Dump the stack of the current task.
 */
void dump_stack(void)
{
	show_stack(NULL, NULL);
}
EXPORT_SYMBOL(dump_stack);

/*
 * show_stack()
 *	Print out information from the current stack.
 */
void show_stack(struct task_struct *task, unsigned long *sp)
{
	/*
	 *  Allocate just enough entries on the stack.
	 */
	unsigned int calls[TRAP_MAX_STACK_DEPTH];
	unsigned long code_start;
	unsigned long code_end;
	unsigned long ocm_code_start = (unsigned long)&__ocm_text_run_begin;
	unsigned long ocm_code_end = (unsigned long)&__data_begin;
	unsigned long stack_end = (unsigned long)(current->stack + THREAD_SIZE - 8);
	unsigned long stack = (unsigned long)sp;
	int kernel_stack = 1;

	processor_dram(&code_start, &code_end);

	/*
	 * Which task are we talking about.
	 */
	if (!task) {
		task = current;
	}

	/*
	 * Find the stack for the task if one was not specified.  Otherwise
	 * use the specified stack.
	 */
	if (!stack) {
		if (task != current) {
			stack = task->thread.sp;
			stack_end = (unsigned long)task->stack + THREAD_SIZE - 8;
		} else {
			asm volatile (
				"move.4		%0, SP		\n\t"
				: "=r" (stack)
			);
		}
	}

	printk(KERN_NOTICE "Starting backtrace: PID %d '%s'\n",
			task->pid, task->comm);

	/*
	 * We do 2 passes the first pass is Kernel stack is the second
	 * User stack.
	 */
	while (kernel_stack) {
		unsigned long *handle;
		unsigned int i, idx = 0;
		struct pt_regs *pt = task_pt_regs(task);

		/*
		 * If the task is in user mode, reset the start
		 * and end values for text.
		 */
		if (__user_mode(stack)) {
			if (!(task->personality & FDPIC_FUNCPTRS)) {
				printk(KERN_NOTICE "  User Stack:\n");
				code_start = task->mm->start_code;
				code_end = task->mm->end_code;
			} else {
				printk(KERN_NOTICE "  User Stack (fdpic):\n");
				show_vmas(task);
			}
			stack_end = task->mm->start_stack;
			ocm_code_end = ocm_code_start = 0;
			kernel_stack = 0;
		} else {
			printk(KERN_NOTICE "  Kernel Stack:\n");
		}

		/*
		 * Collect the stack back trace information.
		 */
		printk("    code[0x%lx-0x%lx]", code_start, code_end);
		if (ocm_code_start) {
			printk(" ocm_code[0x%lx-0x%lx]",
			       ocm_code_start, ocm_code_end);
		}
		printk("\n    stack[0x%lx-0x%lx]\n", stack, stack_end);

		handle = (unsigned long*)stack;
		while (idx < TRAP_MAX_STACK_DEPTH) {
			calls[idx] = stacktrace_iterate(&handle,
					code_start, code_end,
					ocm_code_start, ocm_code_end,
					(unsigned long)stack, stack_end);
			if (calls[idx] == 0) {
				break;
			}
			idx++;
		}

		/*
		 * Now print out the data.
		 */
		printk(KERN_NOTICE "  CALL && CALLI on stack:");
		for (i = 0; i < idx; i++) {
			printk("%s0x%x, ", (i & 0x3) == 0 ?  "\n    " : "",
					calls[i]);
		}
		printk(idx == TRAP_MAX_STACK_DEPTH ? "...\n" : "\n");

		/*
		 * If we are doing user stack we are done
		 */
		if (!kernel_stack) {
			break;
		}

		/*
		 * Does this kernel stack have a mm (i.e. is it user)
		 */
		if (!task->mm) {
			printk("No mm for userspace stack.\n");
			break;
		}
		/*
		 * Get the user-mode stack (if any)
		 */
		stack = pt->an[7];
		printk(KERN_NOTICE "Userspace stack at 0x%lx frame type %d\n",
				stack, (int)pt->frame_type);
		if (!__user_mode(stack)) {
			break;
		}
	}
}

/*
 * die_if_kernel()
 *	Determine if we are in kernel mode and if so print stuff out and die.
 */
void die_if_kernel(char *str, struct pt_regs *regs, long trap_cause)
{
	unsigned int s3value;

	if (user_mode(regs)) {
		return;
	}

	console_verbose();
	trap_print_information(str, regs);

	/*
	 * If the debugger is attached via the hardware mailbox protocol,
	 * go into an infinite loop and the debugger will figure things out.
	 */
	asm volatile (
	      "move.4 %0, scratchpad3"
	      : "=r" (s3value)
	);
	if (s3value) {
		asm volatile("1:	jmpt.t 1b");
	}

	/*
	 * Set the debug taint value.
	 */
	add_taint(TAINT_DIE);
	do_exit(SIGSEGV);
}

/*
 * trap_handler()
 *	Handle traps.
 *
 * Traps are treated as interrupts and registered with the LDSR.  When
 * the LDSR takes the interrupt, it will determine if a trap has occurred
 * and service the trap prior to servicing the interrupt.
 *
 * This function is directly called by the LDSR.
 */
void trap_handler(int irq, struct pt_regs *regs)
{
	int sig = SIGSEGV;
	siginfo_t info;
	unsigned int trap_cause = regs->trap_cause;

	BUG_ON(!irqs_disabled());

	/*
	 * test if in kernel and die.
	 */
	die_if_kernel("Kernel Trap", regs, trap_cause);

	/*
	 * User process problem, setup a signal for this process
	 */
	if ((trap_cause & (1 << TRAP_CAUSE_DST_RANGE_ERR)) ||
	    (trap_cause & (1 << TRAP_CAUSE_SRC1_RANGE_ERR)) ||
	    (trap_cause & (1 << TRAP_CAUSE_I_RANGE_ERR))) {
		sig = SIGSEGV;
		info.si_code = SEGV_MAPERR;
	} else if ((trap_cause & (1 << TRAP_CAUSE_DST_MISALIGNED)) ||
		   (trap_cause & (1 << TRAP_CAUSE_SRC1_MISALIGNED))) {
		sig = SIGBUS;
		info.si_code = BUS_ADRALN;
	} else if ((trap_cause & (1 << TRAP_CAUSE_DST_DECODE_ERR)) ||
		   (trap_cause & (1 << TRAP_CAUSE_SRC1_DECODE_ERR))) {
		sig = SIGILL;
		info.si_code = ILL_ILLOPN;
	} else if ((trap_cause & (1 << TRAP_CAUSE_ILLEGAL_INST))) {
		/*
		 * Check for software break point and if found signal trap
		 * not illegal instruction.
		 */
		unsigned long instruction;
		if (between(regs->pc, KERNELSTART, memory_end) &&
			(regs->pc & 3) == 0 &&
			get_user(instruction, (unsigned long *)regs->pc) == 0) {

			/*
			 * This used to be 0xaabbccdd but it turns out
			 * that is now valid in ubicom32v4 isa so we
			 * have switched to 0xfabbccdd
			 */
			if ((instruction == 0xfabbccdd) ||
			    (instruction == 0xaabbccdd)) {
				sig = SIGTRAP;
				info.si_code = TRAP_BRKPT;
				goto send_signal;
			}
		}
		sig = SIGILL;
		info.si_code = ILL_ILLOPC;
	} else if ((trap_cause & (1 << TRAP_CAUSE_I_DECODE_ERR))) {
		sig = SIGILL;
		info.si_code = ILL_ILLOPC;
	} else if ((trap_cause & (1 << TRAP_CAUSE_DCAPT))) {
		sig = SIGTRAP;
		info.si_code = TRAP_TRACE;
	}

	/*
	 * Print a trap information block to the console, do not
	 * print this above the case because we don't want it
	 * printed for software break points.
	 */
	trap_print_information("User Trap", regs);

send_signal:

	force_sig_info(sig, &info, current);

	/*
	 * Interrupts are disabled, re-enable them now.
	 */
	if (!irqs_disabled()) {
		printk(KERN_EMERG "interrupts enabled on exit, irq=%d, regs=%p",
				irq, regs);
		BUG();
	}
}

/*
 * trap_init_interrupt()
 *	We need a 2nd trap handling init that will occur after init_IRQ().
 */
void __init trap_init_interrupt(void)
{
	int err;
	unsigned char tirq;
	struct devtree_node *dn = (struct devtree_node *)tn;

	/*
	 * Now setup the Software IRQ so that if a trap occurs the LDSR
	 * is started.  The irq is there just to "force" the LDSR to run.
	 */
	if (!tn) {
		printk(KERN_WARNING "trap_init_interrupt skipped.\n");
		return;
	}

	err = devtree_irq(dn, NULL, &tirq);
	if (err) {
		printk(KERN_WARNING "error obtaining trap irq value: %d\n",
			err);
		return;
	}

	if (tirq == DEVTREE_IRQ_NONE) {
		printk(KERN_WARNING "trap irq not available: %d\n", tirq);
		return;
	}

	err = setup_irq(tirq, &trap_irq);
	if (err) {
		printk(KERN_WARNING "trap irq setup failed: %d\n", err);
		return;
	}

	/*
	 * Let ultra know which thread is handling the traps and
	 * what the interrupt to use is.
	 */
	tn->intthread = ldsr_get_threadid();

	/*
	 * Tell the LDSR about our IRQ so that it will unsuspend
	 * if one occurs while waiting for the per thread lock.
	 */
	ldsr_set_trap_irq(tirq);
}

/*
 * trap_init()
 *	init trap handling
 *
 * Trap handling is done through the ldsr.  Every time an interrupt
 * occurs, the LDSR looks for threads that are listed in the TRAP
 * register and forces a call to the trap handler.
 */
void __init trap_init(void)
{
	/*
	 * If we do not have a trap node in the device tree, we leave the fault
	 * handling to the underlying hardware.
	 */
	tn = (struct trapnode *)devtree_find_node("traps");
	if (!tn) {
		printk(KERN_WARNING "traps are not handled by linux\n");
		return;
	}
}
