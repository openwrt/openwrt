/*
 * arch/ubicom32/kernel/ldsr.c
 *   Ubicom32 architecture Linux Device Services Driver Interface
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
 *
 * NOTES:
 *
 * The LDSR is a programmable interrupt controller that is written in software.
 * It emulates the behavior of an pic by fielding the interrupts, choosing a
 * victim thread to take the interrupt and forcing that thread to take a context
 * switch to the appropriate interrupt handler.
 *
 * Because traps are treated as just a special class of interrupts, the LDSR
 * also handles the processing of traps.
 *
 * Because we compile Linux both UP and SMP, we need the LDSR to use
 * architectural locking that is not "compiled out" when compiling UP.  For now,
 * we use the single atomic bit lock.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/profile.h>
#include <linux/clocksource.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/cpumask.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include <asm/ip5000.h>
#include <asm/atomic.h>
#include <asm/machdep.h>
#include <asm/asm-offsets.h>
#include <asm/traps.h>
#include <asm/thread.h>
#include <asm/range-protect.h>

/*
 * One can not print from the LDSR so the best we can do is
 * check a condition and stall all of the threads.
 */

// #define DEBUG_LDSR 1
#if defined(DEBUG_LDSR)
#define DEBUG_ASSERT(cond) \
	if (!(cond)) { \
		THREAD_STALL; \
	}
#else
#define DEBUG_ASSERT(cond)
#endif

/*
 * Make global so that we can use it in the RFI code in assembly.
 */
unsigned int ldsr_soft_irq_mask;
EXPORT_SYMBOL(ldsr_soft_irq_mask);

static unsigned int ldsr_suspend_mask;
static unsigned int ldsr_soft_irq;
static unsigned int ldsr_stack_space[1024];

static struct ldsr_register_bank {
	volatile unsigned int enabled0;
	volatile unsigned int enabled1;
	volatile unsigned int mask0;
	volatile unsigned int mask1;
	unsigned int total;
	unsigned int retry;
	unsigned int backout;
} ldsr_interrupt;

/*
 * Which thread/cpu are we?
 */
static int ldsr_tid = -1;

#if defined(CONFIG_IRQSTACKS)
/*
 * per-CPU IRQ stacks (thread information and stack)
 *
 * NOTE: Do not use DEFINE_PER_CPU() as it makes it harder
 * to find the location of ctx from assembly language.
 */
union irq_ctx {
	struct thread_info      tinfo;
	u32                     stack[THREAD_SIZE/sizeof(u32)];
};
static union irq_ctx *percpu_irq_ctxs[NR_CPUS];

/*
 *  Storage for the interrupt stack.
 */
#if !defined(CONFIG_IRQSTACKS_USEOCM)
static char percpu_irq_stacks[(NR_CPUS * THREAD_SIZE) + (THREAD_SIZE - 1)];
#else
/*
 *  For OCM, the linker will ensure that space is allocated for the stack
 *  see (vmlinux.lds.S)
 */
static char percpu_irq_stacks[];
#endif

#endif

/*
 * Save trap IRQ because we need to un-suspend if it gets set.
 */
static unsigned int ldsr_trap_irq_mask;
static unsigned int ldsr_trap_irq;

/*
 * ret_from_interrupt_to_kernel
 *	Just restore the context and do nothing else.
 */
asmlinkage void ret_from_interrupt_to_kernel(void)__attribute__((naked));

/*
 * ret_from_interrupt_to_user
 *	Call scheduler if needed. Just restore the context.
 */
asmlinkage void ret_from_interrupt_to_user(void)__attribute__((naked));

#ifdef DEBUG_LDSR
u32_t old_sp, old_pc, old_a0, old_a5, old_a3;
struct pt_regs copy_regs, *copy_save_area;
#endif

int __user_mode(unsigned long sp)
{

	u32_t saved_stack_base = sp & ~(ASM_THREAD_SIZE - 1);
#if defined(CONFIG_IRQSTACKS_USEOCM)
	if ((union irq_ctx *)saved_stack_base == percpu_irq_ctxs[smp_processor_id()]) {
		/*
		 *  On the interrupt stack.
		 */
		return 0;
	}
#endif

	if (!(u32_t)current) {
		return 0;
	}
	return saved_stack_base != ((u32_t)current->stack);
}

/*
 * ldsr_lock_release()
 *	Release the LDSR lock.
 */
static void ldsr_lock_release(void)
{
	UBICOM32_UNLOCK(LDSR_LOCK_BIT);
}

/*
 * ldsr_lock_acquire()
 *	Acquire the LDSR lock, spin if not available.
 */
static void ldsr_lock_acquire(void)
{
	UBICOM32_LOCK(LDSR_LOCK_BIT);
}

/*
 * ldsr_thread_irq_disable()
 *	Disable interrupts for the specified thread.
 */
static void ldsr_thread_irq_disable(unsigned int tid)
{
	unsigned int mask = (1 << tid);

	asm volatile (
	"	or.4	scratchpad1, scratchpad1, %0	\n\t"
		:
		: "d"(mask)
		: "cc"
	);
}

/*
 * ldsr_thread_get_interrupts()
 *	Get the interrupt state for all threads.
 */
static unsigned long ldsr_thread_get_interrupts(void)
{
	unsigned long ret = 0;
	asm volatile (
	"	move.4	%0, scratchpad1	\n\t"
		: "=r" (ret)
		:
	);
	return ret;
}

/*
 * ldsr_emulate_and_run()
 *	Emulate the instruction and then set the thread to run.
 */
static void ldsr_emulate_and_run(unsigned int tid)
{
	unsigned int thread_mask = (1 << tid);
	u32_t write_csr = (tid << 15) | (1 << 14);

	/*
	 * Emulate the unaligned access.
	 */
	unaligned_emulate(tid);

	/*
	 * Get the thread back in a running state.
	 */
	asm volatile (
	"	setcsr	%0			\n\t"
	"	setcsr_flush 0			\n\t"
	"	move.4	trap_cause, #0		\n\t" /* Clear the trap cause
						       * register */
	"	setcsr	#0			\n\t"
	"	setcsr_flush 0			\n\t"
	"	move.4	mt_dbg_active_set, %1	\n\t" /* Activate thread even if
						       * in dbg/fault state */
	"	move.4	mt_active_set, %1	\n\t" /* Restart target
						       * thread. */
		:
		: "r" (write_csr), "d" (thread_mask)
		: "cc"
	);
	thread_enable_mask(thread_mask);
}

/*
 * ldsr_preemptive_context_save()
 *	save thread context from another hardware thread.  The other thread must
 *	be stalled.
 */
static inline void ldsr_preemptive_context_save(u32_t thread,
						struct pt_regs *regs)
{
	/*
	 * Save the current state of the specified thread
	 */
	asm volatile (
	"       move.4  a3, %0					\n\t"

		/* set src1 from the target thread */
	"       move.4  csr, %1					\n\t"
	"	setcsr_flush 0					\n\t"
	"	setcsr_flush 0					\n\t"

		/* copy state from the other thread */
	"       move.4  "D(PT_D0)"(a3), d0			\n\t"
	"       move.4  "D(PT_D1)"(a3), d1			\n\t"
	"       move.4  "D(PT_D2)"(a3), d2			\n\t"
	"       move.4  "D(PT_D3)"(a3), d3			\n\t"
	"       move.4  "D(PT_D4)"(a3), d4			\n\t"
	"       move.4  "D(PT_D5)"(a3), d5			\n\t"
	"       move.4  "D(PT_D6)"(a3), d6			\n\t"
	"       move.4  "D(PT_D7)"(a3), d7			\n\t"
	"       move.4  "D(PT_D8)"(a3), d8			\n\t"
	"       move.4  "D(PT_D9)"(a3), d9			\n\t"
	"       move.4  "D(PT_D10)"(a3), d10			\n\t"
	"       move.4  "D(PT_D11)"(a3), d11			\n\t"
	"       move.4  "D(PT_D12)"(a3), d12			\n\t"
	"       move.4  "D(PT_D13)"(a3), d13			\n\t"
	"       move.4  "D(PT_D14)"(a3), d14			\n\t"
	"       move.4  "D(PT_D15)"(a3), d15			\n\t"
	"       move.4  "D(PT_A0)"(a3), a0			\n\t"
	"       move.4  "D(PT_A1)"(a3), a1			\n\t"
	"       move.4  "D(PT_A2)"(a3), a2			\n\t"
	"       move.4  "D(PT_A3)"(a3), a3			\n\t"
	"       move.4  "D(PT_A4)"(a3), a4			\n\t"
	"       move.4  "D(PT_A5)"(a3), a5			\n\t"
	"       move.4  "D(PT_A6)"(a3), a6			\n\t"
	"       move.4  "D(PT_SP)"(a3), a7			\n\t"
	"       move.4  "D(PT_ACC0HI)"(a3), acc0_hi		\n\t"
	"       move.4  "D(PT_ACC0LO)"(a3), acc0_lo		\n\t"
	"       move.4  "D(PT_MAC_RC16)"(a3), mac_rc16		\n\t"
	"       move.4  "D(PT_ACC1HI)"(a3), acc1_hi		\n\t"
	"       move.4  "D(PT_ACC1LO)"(a3), acc1_lo		\n\t"
	"       move.4  "D(PT_SOURCE3)"(a3), source3		\n\t"
	"       move.4  "D(PT_INST_CNT)"(a3), inst_cnt		\n\t"
	"       move.4  "D(PT_CSR)"(a3), csr			\n\t"
	"       move.4  "D(PT_DUMMY_UNUSED)"(a3), #0		\n\t"
	"       move.4  "D(PT_INT_MASK0)"(a3), int_mask0	\n\t"
	"       move.4  "D(PT_INT_MASK1)"(a3), int_mask1	\n\t"
	"       move.4  "D(PT_TRAP_CAUSE)"(a3), trap_cause	\n\t"
	"       move.4  "D(PT_PC)"(a3), pc			\n\t"
	"	move.4	"D(PT_PREVIOUS_PC)"(a3), previous_pc	\n\t"
		/* disable csr thread select */
	"       movei   csr, #0					\n\t"
	"       setcsr_flush 0					\n\t"
	:
	: "r" (regs->dn), "d" ((thread << 9) | (1 << 8))
	: "a3"
	);
}

/*
 * ldsr_rotate_threads()
 *	Simple round robin algorithm for choosing the next cpu
 */
static int ldsr_rotate_threads(unsigned long cpus)
{
	static unsigned char ldsr_bits[8] = {
		3, 0, 1, 0, 2, 0, 1, 0
	};

	static int nextbit;
	int thisbit;

	/*
	 * Move the interrupts down so that we consider interrupts from where
	 * we left off, then take the interrupts we would lose and move them
	 * to the top half of the interrupts value.
	 */
	cpus = (cpus >> nextbit) | (cpus << ((sizeof(cpus) * 8) - nextbit));

	/*
	 * 50% of the time we won't take this at all and then of the cases where
	 * we do about 50% of those we only execute once.
	 */
	if (!(cpus & 0xffff)) {
		nextbit += 16;
		cpus >>= 16;
	}

	if (!(cpus & 0xff)) {
		nextbit += 8;
		cpus >>= 8;
	}

	if (!(cpus & 0xf)) {
		nextbit += 4;
		cpus >>= 4;
	}

	nextbit += ldsr_bits[cpus & 0x7];
	thisbit = (nextbit & ((sizeof(cpus) * 8) - 1));
	nextbit = (thisbit + 1) & ((sizeof(cpus) * 8) - 1);
	DEBUG_ASSERT(thisbit < THREAD_ARCHITECTURAL_MAX);
	return thisbit;
}

/*
 * ldsr_rotate_interrupts()
 *	Get rotating next set bit value.
 */
static int ldsr_rotate_interrupts(unsigned long long interrupts)
{
	static unsigned char ldsr_bits[8] = {
		3, 0, 1, 0, 2, 0, 1, 0
	};

	static int nextbit;
	int thisbit;

	/*
	 * Move the interrupts down so that we consider interrupts from where
	 * we left off, then take the interrupts we would lose and move them
	 * to the top half of the interrupts value.
	 */
	interrupts = (interrupts >> nextbit) |
		(interrupts << ((sizeof(interrupts) * 8) - nextbit));

	/*
	 * 50% of the time we won't take this at all and then of the cases where
	 * we do about 50% of those we only execute once.
	 */
	if (!(interrupts & 0xffffffff)) {
		nextbit += 32;
		interrupts >>= 32;
	}

	if (!(interrupts & 0xffff)) {
		nextbit += 16;
		interrupts >>= 16;
	}

	if (!(interrupts & 0xff)) {
		nextbit += 8;
		interrupts >>= 8;
	}

	if (!(interrupts & 0xf)) {
		nextbit += 4;
		interrupts >>= 4;
	}

	nextbit += ldsr_bits[interrupts & 0x7];
	thisbit = (nextbit & ((sizeof(interrupts) * 8) - 1));
	nextbit = (thisbit + 1) & ((sizeof(interrupts) * 8) - 1);

	DEBUG_ASSERT(thisbit < (sizeof(interrupts) * 8));
	return thisbit;
}

/*
 * ldsr_backout_or_irq()
 *
 * One way or the other this interrupt is not being
 * processed, make sure that it is reset.  We are
 * not going to call irq_end_vector() so unmask the
 * interrupt.
 */
static void ldsr_backout_of_irq(int vector, unsigned long tid_mask)
{
#if defined(CONFIG_SMP)
	if (unlikely(vector == smp_ipi_irq)) {
		smp_reset_ipi(tid_mask);
	}
#endif
	ldsr_unmask_vector(vector);
	ldsr_interrupt.backout++;
}

#if defined(CONFIG_IRQSTACKS)
/*
 * ldsr_choose_savearea_and_returnvec()
 *	Test our current state (user, kernel, interrupt) and set things up.
 *
 * This version of the function uses 3 stacks and nests interrupts
 * on the interrupt stack.
 */
static struct pt_regs *ldsr_choose_savearea_and_returnvec(thread_t tid, u32_t linux_sp, u32_t *pvec)
{
	struct pt_regs *save_area;
	u32_t masked_linux_sp = linux_sp & ~(THREAD_SIZE - 1);
	struct thread_info * ti= (struct thread_info *)sw_ksp[tid];

#if defined(CONFIG_SMP)
	union irq_ctx *icp = percpu_irq_ctxs[tid];
#else
	union irq_ctx *icp = percpu_irq_ctxs[0];
#endif

	if (masked_linux_sp == (u32_t)icp) {
		/*
		 * Fault/Interrupt occurred while on the interrupt stack.
		 */
		save_area = (struct pt_regs *)((char *)linux_sp - sizeof(struct pt_regs) - 8);
		*pvec = (u32_t)(&ret_from_interrupt_to_kernel);
	} else {
		/*
		 *  Fault/Interrupt occurred while on user/kernel stack.  This is a new
		 *  first use of the interrupt stack.
		 */
		save_area = (struct pt_regs *) ((char *)icp + sizeof(icp->stack) - sizeof(struct pt_regs) - 8);
		if (masked_linux_sp == (u32_t)ti) {
			*pvec  = (u32_t)(&ret_from_interrupt_to_kernel);
		} else {
			*pvec  = (u32_t)(&ret_from_interrupt_to_user);
		}

		/*
		 * Because the softirq code will execute on the "interrupt" stack, we
		 * need to maintain the knowledge of what "task" was executing on the
		 * cpu.  This is done by copying the thread_info->task from the cpu
		 * we are about to context switch into the interrupt contexts thread_info
		 * structure.
		 */
		icp->tinfo.task = ti->task;
		icp->tinfo.preempt_count =
				(icp->tinfo.preempt_count & ~SOFTIRQ_MASK) |
				(ti->preempt_count & SOFTIRQ_MASK);
		icp->tinfo.interrupt_nesting = 0;
	}
	save_area->nesting_level = icp->tinfo.interrupt_nesting;
	return save_area;
}

#else
/*
 * ldsr_choose_savearea_and_returnvec()
 *	Test our current state (user, kernel, interrupt) and set things up.
 *
 * The version of the function uses just the user & kernel stack and
 * nests interrupts on the existing kernel stack.
 */
static struct pt_regs *ldsr_choose_savearea_and_returnvec(thread_t tid, u32_t linux_sp, u32_t *pvec)
{
	struct pt_regs *save_area;
	u32_t masked_linux_sp = linux_sp & ~(THREAD_SIZE - 1);
	struct thread_info *ti = (struct thread_info *)sw_ksp[tid];

	if (masked_linux_sp == (u32_t)ti) {
		/*
		 * Fault/Interrupt occurred while on the kernel stack.
		 */
		save_area = (struct pt_regs *)((char *)linux_sp - sizeof(struct pt_regs) - 8);
		*pvec = (u32_t) (&ret_from_interrupt_to_kernel);
	} else {
		/*
		 *  Fault/Interrupt occurred while on user stack.
		 */
		ti->interrupt_nesting = 0;
		save_area = (struct pt_regs *)((u32_t)ti + THREAD_SIZE - sizeof(struct pt_regs) - 8);
		*pvec  = (u32_t) (&ret_from_interrupt_to_user);
	}
	save_area->nesting_level = ti->interrupt_nesting;
	return save_area;
}
#endif

/*
 * ldsr_ctxsw_thread()
 *	Context switch a mainline thread to execute do_IRQ() for the specified
 *	vector.
 */
static void ldsr_ctxsw_thread(int vector, thread_t tid)
{
	u32_t linux_sp;
	u32_t return_vector;
	struct pt_regs *save_area, *regs;
	u32_t thread_mask = (1 << tid);
	u32_t read_csr = ((tid << 9) | (1 << 8));
	u32_t write_csr = (tid << 15) | (1 << 14);
	u32_t interrupt_vector = (u32_t)(&do_IRQ);

	unsigned int frame_type = UBICOM32_FRAME_TYPE_INTERRUPT;


	DEBUG_ASSERT(!thread_is_enabled(tid));

	/*
	 * Acquire the necessary global and per thread locks for tid.
	 * As a side effect, we ensure that the thread has not trapped
	 * and return true if it has.
	 */
	if (unlikely(thread_is_trapped(tid))) {
		/*
		 * Read the trap cause, the sp and clear the MT_TRAP bits.
		 */
		unsigned int cause;
		asm volatile (
		"	setcsr	%3		\n\t"
		"	setcsr_flush 0		\n\t"
		"	setcsr_flush 0		\n\t"
		"	move.4	%0, TRAP_CAUSE	\n\t"
		"	move.4	%1, SP		\n\t"
		"	setcsr	#0		\n\t"
		"	setcsr_flush 0		\n\t"
		"	move.4	MT_BREAK_CLR, %2\n\t"
		"	move.4	MT_TRAP_CLR, %2	\n\t"
			: "=&r" (cause), "=&r" (linux_sp)
			: "r" (thread_mask), "m" (read_csr)
		);

		ldsr_backout_of_irq(vector, (1 << tid));

#if !defined(CONFIG_UNALIGNED_ACCESS_DISABLED)
		/*
		 * See if the unaligned trap handler can deal with this.
		 * If so, emulate the instruction and then just restart
		 * the thread.
		 */
		if (unaligned_only(cause)) {
#if defined(CONFIG_UNALIGNED_ACCESS_USERSPACE_ONLY)
			/*
			 * Check if this is a kernel stack if so we will not
			 * handle the trap
			 */
			u32_t masked_linux_sp = linux_sp & ~(THREAD_SIZE - 1);
			if ((masked_linux_sp != (u32_t)sw_ksp[tid]) &&
			    unaligned_only(cause)) {
				ldsr_emulate_and_run(tid);
				return;
			}
#else
			ldsr_emulate_and_run(tid);
			return;
#endif

		}
#endif

		interrupt_vector = (u32_t)(&trap_handler);
		frame_type = UBICOM32_FRAME_TYPE_TRAP;
	} else {
		/*
		 * Read the target thread's SP
		 */
		asm volatile (
		"	setcsr	%1		\n\t"
		"	setcsr_flush 0		\n\t"
		"	setcsr_flush 0		\n\t"
		"	move.4	%0, SP		\n\t"
		"	setcsr	#0		\n\t"
		"	setcsr_flush 0		\n\t"
			: "=m" (linux_sp)
			: "m" (read_csr)
		);
	}

	/*
	 * We are delivering an interrupt, count it.
	 */
	ldsr_interrupt.total++;

	/*
	 * At this point, we will definitely force this thread to
	 * a new context, show its interrupts as disabled.
	 */
	ldsr_thread_irq_disable(tid);

	/*
	 * Test our current state (user, kernel, interrupt).  Save the
	 * appropriate data and setup for the return.
	 */
	save_area = ldsr_choose_savearea_and_returnvec(tid, linux_sp, &return_vector);

	/*
	 *  The pt_regs (save_area) contains the type of thread that we are dealing
	 *  with (KERNEL/NORMAL) and is copied into each pt_regs area.  We get this
	 *  from the current tasks kernel pt_regs area that always exists at the
	 *  top of the kernel stack.
	 */
	regs = (struct pt_regs *)((u32_t)sw_ksp[tid] + THREAD_SIZE - sizeof(struct pt_regs) - 8);
	save_area->thread_type = regs->thread_type;

	/*
	 * Preserve the context of the Linux thread.
	 */
	ldsr_preemptive_context_save(tid, save_area);

	/*
	 * Load the fram_type into the save_area.
	 */
	save_area->frame_type = frame_type;

#ifdef CONFIG_STOP_ON_TRAP
	/*
	 * Before we get backtrace and showing stacks working well, it sometimes
	 * helps to enter the debugger when a trap occurs before we change the
	 * thread to handle the fault.  This optional code causes all threads to
	 * stop on every trap frame.  One assumes that GDB connected via the
	 * mailbox interface will be used to recover from this state.
	 */
	if (frame_type == UBICOM32_FRAME_TYPE_TRAP) {
		THREAD_STALL;
	}
#endif

#ifdef DEBUG_LDSR
	copy_regs = *save_area;
	copy_save_area = save_area;

	old_a0 = save_area->an[0];
	old_a3 = save_area->an[3];
	old_sp = save_area->an[7];
	old_a5 = save_area->an[5];
	old_pc = save_area->pc;
#endif

	/*
	 * Now we have to switch the kernel thread to run do_IRQ function.
	 *	Set pc to do_IRQ
	 *	Set d0 to vector
	 *	Set d1 to save_area.
	 *	Set a5 to the proper return vector.
	 */
	asm volatile (
	"	setcsr	%0			\n\t"
	"	setcsr_flush 0			\n\t"
	"	move.4	d0, %5			\n\t" /* d0 = 0 vector # */
	"	move.4	d1, %1			\n\t" /* d1 = save_area */
	"	move.4	sp, %1			\n\t" /* sp = save_area */
	"	move.4	a5, %2			\n\t" /* a5 = return_vector */
	"	move.4	pc, %3			\n\t" /* pc = do_IRQ routine. */
	"	move.4	trap_cause, #0		\n\t" /* Clear the trap cause
						       * register */
	"	setcsr	#0			\n\t"
	"	setcsr_flush 0			\n\t"
	"	enable_kernel_ranges %4		\n\t"
	"	move.4	mt_dbg_active_set, %4	\n\t" /* Activate thread even if
						       * in dbg/fault state */
	"	move.4	mt_active_set, %4	\n\t" /* Restart target
						       * thread. */
		:
		: "r" (write_csr), "r" (save_area),
		  "r" (return_vector), "r" (interrupt_vector),
		  "d" (thread_mask), "r" (vector)
		: "cc"
	);
	thread_enable_mask(thread_mask);
}

/*
 * ldsr_deliver_interrupt()
 *	Deliver the interrupt to one of the threads or all of the threads.
 */
static void ldsr_deliver_interrupt(int vector,
				   unsigned long deliver_to,
				   int all)
{
	unsigned long disabled_threads;
	unsigned long possible_threads;
	unsigned long trapped_threads;
	unsigned long global_locks;

	/*
	 * Disable all of the threads that we might want to send
	 * this interrupt to.
	 */
retry:
	DEBUG_ASSERT(deliver_to);
	thread_disable_mask(deliver_to);

	/*
	 * If any threads are in the trap state, we have to service the
	 * trap for those threads first.
	 */
	asm volatile (
		"move.4	%0, MT_TRAP		\n\t"
		: "=r" (trapped_threads)
		:
	);

	trapped_threads &= deliver_to;
	if (unlikely(trapped_threads)) {
		/*
		 * all traps will be handled, so clear the trap bit before restarting any threads
		 */
	        ubicom32_clear_interrupt(ldsr_trap_irq);

		/*
		 * Let the remaining untrapped threads, continue.
		 */
		deliver_to &= ~trapped_threads;
		if (deliver_to) {
			thread_enable_mask(deliver_to);
		}

		/*
		 * For the trapped threads force them to handle
		 * a trap.
		 */
		while (trapped_threads) {
			unsigned long which = ffz(~trapped_threads);
			trapped_threads &= ~(1 << which);
			ldsr_ctxsw_thread(vector, which);
		}
		return;
	}

	/*
	 * Can we deliver an interrupt to any of the threads?
	 */
	disabled_threads = ldsr_thread_get_interrupts();
	possible_threads = deliver_to & ~disabled_threads;
	if (unlikely(!possible_threads)) {
#if defined(CONFIG_SMP)
		/*
		 * In the SMP case, we can not wait because 1 cpu might be
		 * sending an IPI to another cpu which is currently blocked.
		 * The only way to ensure IPI delivery is to backout and
		 * keep trying.  For SMP, we don't sleep until the interrupts
		 * are delivered.
		 */
		thread_enable_mask(deliver_to);
		ldsr_backout_of_irq(vector, deliver_to);
		return;
#else
		/*
		 * In the UP case, we have nothing to do so we should wait.
		 *
		 * Since the INT_MASK0 and INT_MASK1 are "re-loaded" before we
		 * suspend in the outer loop, we do not need to save them here.
		 *
		 * We test that we were awakened for our specific interrupts
		 * because the ldsr mask/unmask operations will force the ldsr
		 * awake even if the interrupt on the mainline thread is not
		 * completed.
		 */
		unsigned int scratch = 0;
		thread_enable_mask(deliver_to);
		asm volatile (
		"	move.4	INT_MASK0, %1		\n\t"
		"	move.4	INT_MASK1, #0		\n\t"

		"1:	suspend				\n\t"
		"	move.4	%0, INT_STAT0		\n\t"
		"	and.4	%0, %0, %1		\n\t"
		"	jmpeq.f	1b			\n\t"

		"	move.4	INT_CLR0, %2		\n\t"
			: "+r" (scratch)
			: "d" (ldsr_suspend_mask), "r" (ldsr_soft_irq_mask)
			: "cc"
		);

		/*
		 * This delay is sized to coincide with the time it takes a
		 * thread to complete the exit (see return_from_interrupt).
		 */
		ldsr_interrupt.retry++;
		__delay(10);
		goto retry;
#endif
	}

	/*
	 * If any of the global locks are held, we can not deliver any
	 * interrupts, we spin delay(10) and then try again.  If our
	 * spinning becomes a bottle neck, we will need to suspend but for
	 * now lets just spin.
	 */
	asm volatile (
		"move.4	%0, scratchpad1		\n\t"
		: "=r" (global_locks)
		:
	);
	if (unlikely(global_locks & 0xffff0000)) {
		thread_enable_mask(deliver_to);

		/*
		 * This delay is sized to coincide with the average time it
		 * takes a thread to release a global lock.
		 */
		ldsr_interrupt.retry++;
		__delay(10);
		goto retry;
	}

	/*
	 * Deliver to one cpu.
	 */
	if (!all) {
		/*
		 * Find our victim and then enable everyone else.
		 */
		unsigned long victim = ldsr_rotate_threads(possible_threads);
		DEBUG_ASSERT((deliver_to & (1 << victim)));
		DEBUG_ASSERT((possible_threads & (1 << victim)));

		deliver_to &= ~(1 << victim);
		if (deliver_to) {
			thread_enable_mask(deliver_to);
		}
		ldsr_ctxsw_thread(vector, victim);
		return;
	}

	/*
	 * If we can't deliver to some threads, wake them
	 * back up and reset things to deliver to them.
	 */
	deliver_to &= ~possible_threads;
	if (unlikely(deliver_to)) {
		thread_enable_mask(deliver_to);
		ldsr_backout_of_irq(vector, deliver_to);
	}

	/*
	 * Deliver to all possible threads(s).
	 */
	while (possible_threads) {
		unsigned long victim = ffz(~possible_threads);
		possible_threads &= ~(1 << victim);
		ldsr_ctxsw_thread(vector, victim);
	}
}

/*
 * ldsr_thread()
 *	This thread acts as the interrupt controller for Linux.
 */
static void ldsr_thread(void *arg)
{
	int stat0;
	int stat1;
	int interrupt0;
	int interrupt1;
	long long interrupts;
	unsigned long cpus;

#if !defined(CONFIG_SMP)
	/*
	 * In a non-smp configuration, we can not use the cpu(s) arrays because
	 * there is not a 1-1 correspondence between cpus(s) and our threads.
	 * Thus we must get a local idea of the mainline threads and use the
	 * one and only 1 set as the victim.  We do this once before the ldsr
	 * loop.
	 *
	 * In the SMP case, we will use the cpu(s) map to determine which cpu(s)
	 * are valid to send interrupts to.
	 */
	int victim = 0;
	unsigned int mainline = thread_get_mainline();
	if (mainline == 0) {
		panic("no mainline Linux threads to interrupt");
		return;
	}
	victim = ffz(~mainline);
	cpus = (1 << victim);
#endif

	while (1) {
		/*
		 * If one changes this code not to reload the INT_MASK(s), you
		 * need to know that code in the lock waiting above does not
		 * reset the MASK registers back; so that code will need to be
		 * changed.
		 */
		ldsr_lock_acquire();
		asm volatile (
		"	move.4 INT_MASK0, %0	\n\t"
		"	move.4 INT_MASK1, %1	\n\t"
			:
			: "U4" (ldsr_interrupt.mask0), "U4" (ldsr_interrupt.mask1)
		);
		ldsr_lock_release();
		thread_suspend();

		/*
		 * Read the interrupt status registers
		 */
		asm volatile (
			"move.4 %0, INT_STAT0	\n\t"
			"move.4 %1, INT_STAT1	\n\t"
			: "=r" (stat0), "=r" (stat1)
			:
		);

		/*
		 * We only care about interrupts that we have been told to care
		 * about.  The interrupt must be enabled, unmasked, and have
		 * occurred in the hardware.
		 */
		ldsr_lock_acquire();
		interrupt0 = ldsr_interrupt.enabled0 &
			ldsr_interrupt.mask0 & stat0;
		interrupt1 = ldsr_interrupt.enabled1 &
			ldsr_interrupt.mask1 & stat1;
		ldsr_lock_release();

		/*
		 * For each interrupt in the "snapshot" we will mask the
		 * interrupt handle the interrupt (typically calling do_IRQ()).
		 *
		 * The interrupt is unmasked by desc->chip->end() function in
		 * the per chip generic interrupt handling code
		 * (arch/ubicom32/kernel/irq.c).8
		 */
		interrupts = ((unsigned long long)interrupt1 << 32) |
			interrupt0;
		while (interrupts) {
			int all = 0;
			int vector = ldsr_rotate_interrupts(interrupts);
			interrupts &= ~((unsigned long long)1 << vector);

			/*
			 * Now mask off this vector so that the LDSR ignores
			 * it until it is acknowledged.
			 */
			ldsr_mask_vector(vector);
#if !defined(CONFIG_SMP)
			ldsr_deliver_interrupt(vector, cpus, all);
#else
			cpus = smp_get_affinity(vector, &all);
			if (!cpus) {
				/*
				 * No CPU to deliver to so just leave
				 * the interrupt unmasked and increase
				 * the backout count.  We will eventually
				 * return and deliver it again.
				 */
				ldsr_unmask_vector(vector);
				ldsr_interrupt.backout++;
				continue;
			}
			ldsr_deliver_interrupt(vector, cpus, all);
#endif
		}
	}

	/* NOTREACHED */
}

/*
 * ldsr_mask_vector()
 *	Temporarily mask the interrupt vector, turn off the bit in the mask
 *	register.
 */
void ldsr_mask_vector(unsigned int vector)
{
	unsigned int mask;
	if (vector < 32) {
		mask = ~(1 << vector);
		ldsr_lock_acquire();
		ldsr_interrupt.mask0 &= mask;
		ldsr_lock_release();
		thread_resume(ldsr_tid);
		return;
	}

	mask = ~(1 << (vector - 32));
	ldsr_lock_acquire();
	ldsr_interrupt.mask1 &= mask;
	ldsr_lock_release();
	thread_resume(ldsr_tid);
}

/*
 * ldsr_unmask_vector()
 *	Unmask the interrupt vector so that it can be used, turn on the bit in
 *	the mask register.
 *
 * Because it is legal for the interrupt path to disable an interrupt,
 * the unmasking code must ensure that disabled interrupts are not
 * unmasked.
 */
void ldsr_unmask_vector(unsigned int vector)
{
	unsigned int mask;
	if (vector < 32) {
		mask = (1 << vector);
		ldsr_lock_acquire();
		ldsr_interrupt.mask0 |= (mask & ldsr_interrupt.enabled0);
		ldsr_lock_release();
		thread_resume(ldsr_tid);
		return;
	}

	mask = (1 << (vector - 32));
	ldsr_lock_acquire();
	ldsr_interrupt.mask1 |= (mask & ldsr_interrupt.enabled1);
	ldsr_lock_release();
	thread_resume(ldsr_tid);
}

/*
 * ldsr_enable_vector()
 *	The LDSR implements an interrupt controller and has a local (to the
 *	LDSR) copy of its interrupt mask.
 */
void ldsr_enable_vector(unsigned int vector)
{
	unsigned int mask;
	if (vector < 32) {
		mask = (1 << vector);
		ldsr_lock_acquire();
		ldsr_interrupt.enabled0 |= mask;
		ldsr_interrupt.mask0 |= mask;
		ldsr_lock_release();
		thread_resume(ldsr_tid);
		return;
	}

	mask = (1 << (vector - 32));
	ldsr_lock_acquire();
	ldsr_interrupt.enabled1 |= mask;
	ldsr_interrupt.mask1 |= mask;
	ldsr_lock_release();
	thread_resume(ldsr_tid);
}

/*
 * ldsr_disable_vector()
 *	The LDSR implements an interrupt controller and has a local (to the
 *	LDSR) copy of its interrupt mask.
 */
void ldsr_disable_vector(unsigned int vector)
{
	unsigned int mask;

	if (vector < 32) {
		mask = ~(1 << vector);
		ldsr_lock_acquire();
		ldsr_interrupt.enabled0 &= mask;
		ldsr_interrupt.mask0 &= mask;
		ldsr_lock_release();
		thread_resume(ldsr_tid);
		return;
	}

	mask = ~(1 << (vector - 32));
	ldsr_lock_acquire();
	ldsr_interrupt.enabled1 &= mask;
	ldsr_interrupt.mask1 &= mask;
	ldsr_lock_release();
	thread_resume(ldsr_tid);
}

/*
 * ldsr_get_threadid()
 *	Return the threadid of the LDSR thread.
 */
thread_t ldsr_get_threadid(void)
{
	return ldsr_tid;
}

/*
 * ldsr_set_trap_irq()
 *	Save away the trap Soft IRQ
 *
 * See the per thread lock suspend code above for an explination.
 */
void ldsr_set_trap_irq(unsigned int irq)
{
	ldsr_trap_irq = irq;
	ldsr_trap_irq_mask = (1 << irq);
	ldsr_suspend_mask |= ldsr_trap_irq_mask;
}

/*
 * ldsr_init()
 *	Initialize the LDSR (Interrupt Controller)
 */
void ldsr_init(void)
{
#if defined(CONFIG_IRQSTACKS)
	int i;
	union irq_ctx *icp;
#endif

	void *stack_high = (void *)ldsr_stack_space;
	stack_high += sizeof(ldsr_stack_space);
	stack_high -= 8;


	/*
	 * Obtain a soft IRQ to use
	 */
	if (irq_soft_alloc(&ldsr_soft_irq) < 0) {
		panic("no software IRQ is available\n");
		return;
	}
	ldsr_soft_irq_mask |= (1 << ldsr_soft_irq);
	ldsr_suspend_mask |= ldsr_soft_irq_mask;

	/*
	 * Now allocate and start the LDSR thread.
	 */
	ldsr_tid = thread_alloc();
	if (ldsr_tid < 0) {
		panic("no thread available to run LDSR");
		return;
	}

#if defined(CONFIG_IRQSTACKS)
	/*
	 * Initialize the per-cpu irq thread_info structure that
	 * is at the top of each per-cpu irq stack.
	 */
	icp = (union irq_ctx *)
		(((unsigned long)percpu_irq_stacks + (THREAD_SIZE - 1)) & ~(THREAD_SIZE - 1));
	for (i = 0; i < NR_CPUS; i++) {
		struct thread_info *ti = &(icp->tinfo);
		ti->task = NULL;
		ti->exec_domain = NULL;
		ti->cpu = i;
		ti->preempt_count = 0;
		ti->interrupt_nesting = 0;
		percpu_irq_ctxs[i] = icp++;
	}
#endif
	thread_start(ldsr_tid, ldsr_thread, NULL,
		     stack_high, THREAD_TYPE_NORMAL);
}
