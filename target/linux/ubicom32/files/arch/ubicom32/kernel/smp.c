/*
 * arch/ubicom32/kernel/smp.c
 *   SMP implementation for Ubicom32 processors.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 1999 Walt Drummond <drummond@valinux.com>
 * Copyright (C) 1999 David Mosberger-Tang <davidm@hpl.hp.com>
 * Copyright (C) 2001,2004 Grant Grundler <grundler@parisc-linux.org>
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

#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

#include <linux/kernel.h>
#include <linux/bootmem.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/kernel_stat.h>
#include <linux/mm.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/cpu.h>
#include <linux/profile.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/irq.h>

#include <asm/system.h>
#include <asm/atomic.h>
#include <asm/current.h>
#include <asm/tlbflush.h>
#include <asm/timex.h>
#include <asm/cpu.h>
#include <asm/irq.h>
#include <asm/processor.h>
#include <asm/thread.h>
#include <asm/sections.h>
#include <asm/ip5000.h>

/*
 * Mask the debug printout for IPI because they are too verbose
 * for regular debugging.
 */

// #define DEBUG_SMP 1
#if !defined(DEBUG_SMP)
#define smp_debug(lvl, ...)
#else
static unsigned int smp_debug_lvl = 50;
#define smp_debug(lvl, printargs...)		\
	if (lvl >= smp_debug_lvl) {		\
			printk(printargs);	\
	}
#endif

#if !defined(DEBUG_SMP)
#define DEBUG_ASSERT(cond)
#else
#define DEBUG_ASSERT(cond) \
	if (!(cond)) { \
		THREAD_STALL; \
	}
#endif

/*
 * List of IPI Commands (more than one can be set at a time).
 */
enum ipi_message_type {
	IPI_NOP,
	IPI_RESCHEDULE,
	IPI_CALL_FUNC,
	IPI_CALL_FUNC_SINGLE,
	IPI_CPU_STOP,
	IPI_CPU_TIMER,
};

/*
 * We maintain a hardware thread oriented view of online threads
 * and those involved or needing IPI.
 */
static volatile unsigned long smp_online_threads = 0;
static volatile unsigned long smp_needs_ipi = 0;
static volatile unsigned long smp_inside_ipi = 0;
static unsigned long smp_irq_affinity[NR_IRQS];

/*
 * What do we need to track on a per cpu/thread basis?
 */
DEFINE_PER_CPU(struct cpuinfo_ubicom32, cpu_data);

/*
 * Each thread cpuinfo IPI information is guarded by a lock
 * that is kept local to this file.
 */
DEFINE_PER_CPU(spinlock_t, ipi_lock) = SPIN_LOCK_UNLOCKED;

/*
 * The IPI(s) are based on a software IRQ through the LDSR.
 */
unsigned int smp_ipi_irq;

/*
 * Define a spinlock so that only one cpu is able to modify the
 * smp_needs_ipi and to set/clear the IRQ at a time.
 */
DEFINE_SPINLOCK(smp_ipi_lock);

/*
 * smp_halt_processor()
 *	Halt this hardware thread.
 */
static void smp_halt_processor(void)
{
	int cpuid = thread_get_self();
	cpu_clear(smp_processor_id(), cpu_online_map);
	local_irq_disable();
	printk(KERN_EMERG "cpu[%d] has halted. It is not OK to turn off power \
		until all cpu's are off.\n", cpuid);
	for (;;) {
		thread_suspend();
	}
}

/*
 * ipi_interrupt()
 *	Handle an Interprocessor Interrupt.
 */
static irqreturn_t ipi_interrupt(int irq, void *dev_id)
{
	int cpuid = smp_processor_id();
	struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, cpuid);
	unsigned long ops;

	/*
	 * Count this now; we may make a call that never returns.
	 */
	p->ipi_count++;

	/*
	 * We are about to process all ops.  If another cpu has stated
	 * that we need an IPI, we will have already processed it.  By
	 * clearing our smp_needs_ipi, and processing all ops,
	 * we reduce the number of IPI interrupts.  However, this introduces
	 * the possibility that smp_needs_ipi will be clear and the soft irq
	 * will have gone off; so we need to make the get_affinity() path
	 * tolerant of spurious interrupts.
	 */
	spin_lock(&smp_ipi_lock);
	smp_needs_ipi &= ~(1 << p->tid);
	spin_unlock(&smp_ipi_lock);

	for (;;) {
		/*
		 * Read the set of IPI commands we should handle.
		 */
		spinlock_t *lock = &per_cpu(ipi_lock, cpuid);
		spin_lock(lock);
		ops = p->ipi_pending;
		p->ipi_pending = 0;
		spin_unlock(lock);

		/*
		 * If we have no IPI commands to execute, break out.
		 */
		if (!ops) {
			break;
		}

		/*
		 * Execute the set of commands in the ops word, one command
		 * at a time in no particular order.  Strip of each command
		 * as we execute it.
		 */
		while (ops) {
			unsigned long which = ffz(~ops);
			ops &= ~(1 << which);

			BUG_ON(!irqs_disabled());
			switch (which) {
			case IPI_NOP:
				smp_debug(100, KERN_INFO "cpu[%d]: "
					  "IPI_NOP\n", cpuid);
				break;

			case IPI_RESCHEDULE:
				/*
				 * Reschedule callback.  Everything to be
				 * done is done by the interrupt return path.
				 */
				smp_debug(200, KERN_INFO "cpu[%d]: "
					  "IPI_RESCHEDULE\n", cpuid);
				break;

			case IPI_CALL_FUNC:
				smp_debug(100, KERN_INFO "cpu[%d]: "
					  "IPI_CALL_FUNC\n", cpuid);
				generic_smp_call_function_interrupt();
				break;

			case IPI_CALL_FUNC_SINGLE:
				smp_debug(100, KERN_INFO "cpu[%d]: "
					  "IPI_CALL_FUNC_SINGLE\n", cpuid);
				generic_smp_call_function_single_interrupt();
				break;

			case IPI_CPU_STOP:
				smp_debug(100, KERN_INFO "cpu[%d]: "
					  "IPI_CPU_STOP\n", cpuid);
				smp_halt_processor();
				break;

#if !defined(CONFIG_LOCAL_TIMERS)
			case IPI_CPU_TIMER:
				smp_debug(100, KERN_INFO "cpu[%d]: "
					  "IPI_CPU_TIMER\n", cpuid);
#if defined(CONFIG_GENERIC_CLOCKEVENTS)
				local_timer_interrupt();
#else
				update_process_times(user_mode(get_irq_regs()));
				profile_tick(CPU_PROFILING);
#endif
#endif
				break;

			default:
				printk(KERN_CRIT "cpu[%d]: "
					  "Unknown IPI: %lu\n", cpuid, which);

				return IRQ_NONE;
			}

			/*
			 * Let in any pending interrupts
			 */
			BUG_ON(!irqs_disabled());
			local_irq_enable();
			local_irq_disable();
		}
	}
	return IRQ_HANDLED;
}

/*
 * ipi_send()
 *	Send an Interprocessor Interrupt.
 */
static void ipi_send(int cpu, enum ipi_message_type op)
{
	struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, cpu);
	spinlock_t *lock = &per_cpu(ipi_lock, cpu);
	unsigned long flags;

	/*
	 * We protect the setting of the ipi_pending field and ensure
	 * that the ipi delivery mechanism and interrupt are atomically
	 * handled.
	 */
	spin_lock_irqsave(lock, flags);
	p->ipi_pending |= 1 << op;
	spin_unlock_irqrestore(lock, flags);

	spin_lock_irqsave(&smp_ipi_lock, flags);
	smp_needs_ipi |= (1 << p->tid);
	ubicom32_set_interrupt(smp_ipi_irq);
	spin_unlock_irqrestore(&smp_ipi_lock, flags);
	smp_debug(100, KERN_INFO "cpu[%d]: send: %d\n", cpu, op);
}

/*
 * ipi_send_mask
 *	Send an IPI to each cpu in mask.
 */
static inline void ipi_send_mask(unsigned int op, const struct cpumask mask)
{
	int cpu;
	for_each_cpu_mask(cpu, mask) {
		ipi_send(cpu, op);
	}
}

/*
 * ipi_send_allbutself()
 *	Send an IPI to all threads but ourselves.
 */
static inline void ipi_send_allbutself(unsigned int op)
{
	int self = smp_processor_id();
	struct cpumask result;
	cpumask_copy(&result, &cpu_online_map);
	cpu_clear(self, result);
	ipi_send_mask(op, result);
}

/*
 * smp_enable_vector()
 */
static void smp_enable_vector(unsigned int irq)
{
	ubicom32_clear_interrupt(smp_ipi_irq);
	ldsr_enable_vector(irq);
}

/*
 * smp_disable_vector()
 *	Disable the interrupt by clearing the appropriate bit in the
 *	LDSR Mask Register.
 */
static void smp_disable_vector(unsigned int irq)
{
	ldsr_disable_vector(irq);
}

/*
 * smp_mask_vector()
 */
static void smp_mask_vector(unsigned int irq)
{
	ldsr_mask_vector(irq);
}

/*
 * smp_unmask_vector()
 */
static void smp_unmask_vector(unsigned int irq)
{
	ldsr_unmask_vector(irq);
}

/*
 * smp_end_vector()
 *	Called once an interrupt is completed (reset the LDSR mask).
 */
static void smp_end_vector(unsigned int irq)
{
	struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, smp_processor_id());
	spin_lock(&smp_ipi_lock);
	smp_inside_ipi &= ~(1 << p->tid);
	if (smp_inside_ipi) {
		spin_unlock(&smp_ipi_lock);
		return;
	}
	spin_unlock(&smp_ipi_lock);
	ldsr_unmask_vector(irq);
	smp_debug(100, KERN_INFO "cpu[%d]: unamesk vector\n", smp_processor_id());
}

/*
 * Special hanlder functions for SMP.
 */
static struct irq_chip ubicom32_smp_chip = {
	.name		= "UbicoIPI",
	.startup	= NULL,
	.shutdown	= NULL,
	.enable		= smp_enable_vector,
	.disable	= smp_disable_vector,
	.ack		= NULL,
	.mask		= smp_mask_vector,
	.unmask		= smp_unmask_vector,
	.end		= smp_end_vector,
};

/*
 * smp_reset_ipi()
 *	None of these cpu(s) got their IPI, turn it back on.
 *
 * Note: This is called by the LDSR which is not a full
 * Linux cpu.  Thus you must use the raw form of locks
 * because lock debugging will not work on the partial
 * cpu nature of the LDSR.
 */
void smp_reset_ipi(unsigned long mask)
{
	__raw_spin_lock(&smp_ipi_lock.raw_lock);
	smp_needs_ipi |= mask;
	smp_inside_ipi &= ~mask;
	ubicom32_set_interrupt(smp_ipi_irq);
	__raw_spin_unlock(&smp_ipi_lock.raw_lock);
	smp_debug(100, KERN_INFO "smp: reset IPIs for: 0x%x\n", mask);
}

/*
 * smp_get_affinity()
 *	Choose the thread affinity for this interrupt.
 *
 * Note: This is called by the LDSR which is not a full
 * Linux cpu.  Thus you must use the raw form of locks
 * because lock debugging will not work on the partial
 * cpu nature of the LDSR.
 */
unsigned long smp_get_affinity(unsigned int irq, int *all)
{
	unsigned long mask = 0;

	/*
	 * Most IRQ(s) are delivered in a round robin fashion.
	 */
	if (irq != smp_ipi_irq) {
		unsigned long result = smp_irq_affinity[irq] & smp_online_threads;
		DEBUG_ASSERT(result);
		*all = 0;
		return result;
	}

	/*
	 * This is an IPI request.  Return all cpu(s) scheduled for an IPI.
	 * We also track those cpu(s) that are going to be "receiving" IPI this
	 * round.  When all CPU(s) have called smp_end_vector(),
	 * we will unmask the IPI interrupt.
	 */
	__raw_spin_lock(&smp_ipi_lock.raw_lock);
	ubicom32_clear_interrupt(smp_ipi_irq);
	if (smp_needs_ipi) {
		mask = smp_needs_ipi;
		smp_inside_ipi |= smp_needs_ipi;
		smp_needs_ipi = 0;
	}
	__raw_spin_unlock(&smp_ipi_lock.raw_lock);
	*all = 1;
	return mask;
}

/*
 *  smp_set_affinity()
 *	Set the affinity for this irq but store the value in tid(s).
 */
void smp_set_affinity(unsigned int irq, const struct cpumask *dest)
{
	int cpuid;
	unsigned long *paffinity = &smp_irq_affinity[irq];

	/*
	 *  If none specified, all cpus are allowed.
	 */
	if (cpus_empty(*dest)) {
		*paffinity = 0xffffffff;
		return;
	}

	/*
	 * Make sure to clear the old value before setting up the
	 * list.
	 */
	*paffinity = 0;
	for_each_cpu_mask(cpuid, *dest) {
		struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, cpuid);
		*paffinity |= (1 << p->tid);
	}
}

/*
 * smp_send_stop()
 *	Send a stop request to all CPU but this one.
 */
void smp_send_stop(void)
{
	ipi_send_allbutself(IPI_CPU_STOP);
}

/*
 * smp_send_timer_all()
 *	Send all cpu(s) but this one, a request to update times.
 */
void smp_send_timer_all(void)
{
	ipi_send_allbutself(IPI_CPU_TIMER);
}

/*
 * smp_timer_broadcast()
 *	Use an IPI to broadcast a timer message
 */
void smp_timer_broadcast(const struct cpumask *mask)
{
	ipi_send_mask(IPI_CPU_TIMER, *mask);
}

/*
 * smp_send_reschedule()
 *	Send a reschedule request to the specified cpu.
 */
void smp_send_reschedule(int cpu)
{
	ipi_send(cpu, IPI_RESCHEDULE);
}

/*
 * arch_send_call_function_ipi()
 *	Cause each cpu in the mask to call the generic function handler.
 */
void arch_send_call_function_ipi_mask(const struct cpumask *mask)
{
	int cpu;
	for_each_cpu_mask(cpu, *mask) {
		ipi_send(cpu, IPI_CALL_FUNC);
	}
}

/*
 * arch_send_call_function_single_ipi()
 *	Cause the specified cpu to call the generic function handler.
 */
void arch_send_call_function_single_ipi(int cpu)
{
	ipi_send(cpu, IPI_CALL_FUNC_SINGLE);
}

/*
 * setup_profiling_timer()
 *	Dummy function created to keep Oprofile happy in the SMP case.
 */
int setup_profiling_timer(unsigned int multiplier)
{
	return 0;
}

/*
 * smp_mainline_start()
 *	Start a slave thread executing a mainline Linux context.
 */
static void __init smp_mainline_start(void *arg)
{
	int cpuid = smp_processor_id();
	struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, cpuid);

	BUG_ON(p->tid != thread_get_self());

	/*
	 * Well, support 2.4 linux scheme as well.
	 */
	if (cpu_test_and_set(cpuid, cpu_online_map)) {
		printk(KERN_CRIT "cpu[%d]: already initialized!\n", cpuid);
		smp_halt_processor();
		return;
	}

	/*
	 * Initialise the idle task for this CPU
	 */
	atomic_inc(&init_mm.mm_count);
	current->active_mm = &init_mm;
	if (current->mm) {
		printk(KERN_CRIT "cpu[%d]: idle task already has memory "
		       "management\n", cpuid);
		smp_halt_processor();
		return;
	}

	/*
	 * TODO: X86 does this prior to calling notify, try to understand why?
	 */
	preempt_disable();

#if defined(CONFIG_GENERIC_CLOCKEVENTS)
	/*
	 * Setup a local timer event so that this cpu will get timer interrupts
	 */
	if (local_timer_setup(cpuid) == -1) {
		printk(KERN_CRIT "cpu[%d]: timer alloc failed\n", cpuid);
		smp_halt_processor();
		return;
	}
#endif

	/*
	 * Notify those interested that we are up and alive.  This must
	 * be done before interrupts are enabled.  It must also be completed
	 * before the bootstrap cpu returns from __cpu_up() (see comment
	 * above cpu_set() of the cpu_online_map).
	 */
	notify_cpu_starting(cpuid);

	/*
	 * Indicate that this thread is now online and present.   Setting
	 * cpu_online_map has the side effect of allowing the bootstrap
	 * cpu to continue along; so anything that MUST be done prior to the
	 * bootstrap cpu returning from __cpu_up() needs to go above here.
	 */
	cpu_set(cpuid, cpu_online_map);
	cpu_set(cpuid, cpu_present_map);

	/*
	 * Maintain a thread mapping in addition to the cpu mapping.
	 */
	smp_online_threads |= (1 << p->tid);

	/*
	 * Enable interrupts for this thread.
	 */
	local_irq_enable();

	/*
	 * Enter the idle loop and wait for a timer to schedule some work.
	 */
	printk(KERN_INFO "cpu[%d]: entering cpu_idle()\n", cpuid);
	cpu_idle();

	/* Not Reached */
}

/*
 * smp_cpus_done()
 *	Called once the kernel_init() has brought up all cpu(s).
 */
void smp_cpus_done(unsigned int cpu_max)
{
	/* Do Nothing */
}

/*
 * __cpu_up()
 *	Called to startup a sepcific cpu.
 */
int __cpuinit __cpu_up(unsigned int cpu)
{
	struct task_struct *idle;
	unsigned int *stack;
	long timeout;
	struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, cpu);

	/*
	 * Create an idle task for this CPU.
	 */
	idle = fork_idle(cpu);
	if (IS_ERR(idle)) {
		panic("cpu[%d]: fork failed\n", cpu);
		return -ENOSYS;
	}
	task_thread_info(idle)->cpu = cpu;

	/*
	 * Setup the sw_ksp[] to point to this new task.
	 */
	sw_ksp[p->tid] = (unsigned int)idle->stack;
	stack = (unsigned int *)(sw_ksp[p->tid] + PAGE_SIZE - 8);

	/*
	 * Cause the specified thread to execute our smp_mainline_start
	 * function as a TYPE_NORMAL thread.
	 */
	printk(KERN_INFO "cpu[%d]: launching mainline Linux thread\n", cpu);
	if (thread_start(p->tid, smp_mainline_start, (void *)NULL, stack,
			 THREAD_TYPE_NORMAL) == -1) {
		printk(KERN_WARNING "cpu[%d]: failed thread_start\n", cpu);
		return -ENOSYS;
	}

	/*
	 * Wait for the thread to start up.  The thread will set
	 * the online bit when it is running.  Our caller execpts the
	 * cpu to be online if we return 0.
	 */
	for (timeout = 0; timeout < 10000; timeout++) {
		if (cpu_online(cpu)) {
			break;
		}

		udelay(100);
		barrier();
		continue;
	}

	if (!cpu_online(cpu)) {
		printk(KERN_CRIT "cpu[%d]: failed to live after %ld us\n",
		       cpu, timeout * 100);
		return -ENOSYS;
	}

	printk(KERN_INFO "cpu[%d]: came alive after %ld us\n",
	       cpu, timeout * 100);
	return 0;
}

/*
 * Data used by setup_irq for the IPI.
 */
static struct irqaction ipi_irq = {
	.name	 = "ipi",
	.flags	 = IRQF_DISABLED | IRQF_PERCPU,
	.handler = ipi_interrupt,
};

/*
 * smp_prepare_cpus()
 *	Mark threads that are available to Linux as possible cpus(s).
 */
void __init smp_prepare_cpus(unsigned int max_cpus)
{
	int i;

	/*
	 * We will need a software IRQ to send IPI(s).  We will use
	 * a single software IRQ for all IPI(s).
	 */
	if (irq_soft_alloc(&smp_ipi_irq) < 0) {
		panic("no software IRQ is available\n");
		return;
	}

	/*
	 * For the IPI interrupt, we want to use our own chip definition.
	 * This allows us to define what happens in SMP IPI without affecting
	 * the performance of the other interrupts.
	 *
	 * Next, Register the IPI interrupt function against the soft IRQ.
	 */
	set_irq_chip(smp_ipi_irq, &ubicom32_smp_chip);
	setup_irq(smp_ipi_irq, &ipi_irq);

	/*
	 * We use the device tree node to determine how many
	 * free cpus we will have (up to NR_CPUS) and we indicate
	 * that those cpus are present.
	 *
	 * We need to do this very early in the SMP case
	 * because the Linux init code uses the cpu_present_map.
	 */
	for_each_possible_cpu(i) {
		thread_t tid;
		struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, i);

		/*
		 *  Skip the bootstrap cpu
		 */
		if (i == 0) {
			continue;
		}

		/*
		 * If we have a free thread left in the mask,
		 * indicate that the cpu is present.
		 */
		tid = thread_alloc();
		if (tid == (thread_t)-1) {
			break;
		}

		/*
		 * Save the hardware thread id for this cpu.
		 */
		p->tid = tid;
		cpu_set(i, cpu_present_map);
		printk(KERN_INFO "cpu[%d]: added to cpu_present_map - tid: %d\n", i, tid);
	}
}

/*
 * smp_prepare_boot_cpu()
 *	Copy the per_cpu data into the appropriate spot for the bootstrap cpu.
 *
 * The code in boot_cpu_init() has already set the boot cpu's
 * state in the possible, present, and online maps.
 */
void __devinit smp_prepare_boot_cpu(void)
{
	struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, 0);

	smp_online_threads |= (1 << p->tid);
	printk(KERN_INFO "cpu[%d]: bootstrap CPU online - tid: %ld\n",
			current_thread_info()->cpu, p->tid);
}

/*
 * smp_setup_processor_id()
 *	Set the current_thread_info() structure cpu value.
 *
 * We set the value to the true hardware thread value that we are running on.
 * NOTE: this function overrides the weak alias function in main.c
 */
void __init smp_setup_processor_id(void)
{
	struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, 0);
	int i;
	for_each_cpu_mask(i, CPU_MASK_ALL)
		set_cpu_possible(i, true);

	current_thread_info()->cpu = 0;
	p->tid = thread_get_self();
}
