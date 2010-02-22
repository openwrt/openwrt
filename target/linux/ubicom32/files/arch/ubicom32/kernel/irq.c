/*
 * arch/ubicom32/kernel/irq.c
 *   Ubicom32 architecture IRQ support.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * (C) Copyright 2007, Greg Ungerer <gerg@snapgear.com>
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
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <asm/system.h>
#include <asm/traps.h>
#include <asm/ldsr.h>
#include <asm/ip5000.h>
#include <asm/machdep.h>
#include <asm/asm-offsets.h>
#include <asm/thread.h>
#include <asm/devtree.h>

unsigned int irq_soft_avail;
static struct irqaction ubicom32_reserve_action[NR_IRQS];

#if !defined(CONFIG_DEBUG_IRQMEASURE)
#define IRQ_DECLARE_MEASUREMENT
#define IRQ_MEASUREMENT_START()
#define IRQ_MEASUREMENT_END(irq)
#else
#define IRQ_DECLARE_MEASUREMENT \
	int __diff;		\
	unsigned int __tstart;

#define IRQ_MEASUREMENT_START() \
	__tstart = UBICOM32_IO_TIMER->sysval;

#define IRQ_MEASUREMENT_END(irq) \
	__diff = (int)UBICOM32_IO_TIMER->sysval - (int)__tstart; \
	irq_measurement_update((irq), __diff);

/*
 * We keep track of the time spent in both irq_enter()
 * and irq_exit().
 */
#define IRQ_WEIGHT 32

struct irq_measurement {
	volatile unsigned int min;
	volatile unsigned int avg;
	volatile unsigned int max;
};

static DEFINE_SPINLOCK(irq_measurement_lock);

/*
 *  Add 1 in for softirq (irq_exit());
 */
static struct irq_measurement irq_measurements[NR_IRQS + 1];

/*
 * irq_measurement_update()
 *	Update an entry in the measurement array for this irq.
 */
static void irq_measurement_update(int irq, int sample)
{
	struct irq_measurement *im = &irq_measurements[irq];
	spin_lock(&irq_measurement_lock);
	if ((im->min == 0) || (im->min > sample)) {
		im->min = sample;
	}
	if (im->max < sample) {
		im->max = sample;
	}
	im->avg = ((im->avg * (IRQ_WEIGHT - 1)) + sample) / IRQ_WEIGHT;
	spin_unlock(&irq_measurement_lock);
}
#endif

/*
 * irq_kernel_stack_check()
 *	See if the kernel stack is within STACK_WARN of the end.
 */
static void irq_kernel_stack_check(int irq, struct pt_regs *regs)
{
#ifdef CONFIG_DEBUG_STACKOVERFLOW
	unsigned long sp;

	/*
	 * Make sure that we are not close to the top of the stack and thus
	 * can not really service this interrupt.
	 */
	asm volatile (
		"and.4		%0, SP, %1 \n\t"
		: "=d" (sp)
		: "d" (THREAD_SIZE - 1)
		: "cc"
	);

	if (sp < (sizeof(struct thread_info) + STACK_WARN)) {
		printk(KERN_WARNING
			"cpu[%d]: possible overflow detected sp remain: %p, "
		       "irq: %d, regs: %p\n",
			thread_get_self(), (void *)sp, irq, regs);
		dump_stack();
	}

	if (sp < (sizeof(struct thread_info) + 16)) {
		THREAD_STALL;
	}
#endif
}

/*
 * irq_get_lsb()
 *	Get the LSB set in value
 */
static int irq_get_lsb(unsigned int value)
{
	static unsigned char irq_bits[8] = {
		3, 0, 1, 0, 2, 0, 1, 0
	};
	u32_t nextbit = 0;

	value = (value >> nextbit) | (value << ((sizeof(value) * 8) - nextbit));

	/*
	 * It's unlikely that we find that we execute the body of this while
	 * loop.  50% of the time we won't take this at all and then of the
	 * cases where we do about 50% of those we only execute once.
	 */
	if (!(value & 0xffff)) {
		nextbit += 0x10;
		value >>= 16;
	}

	if (!(value & 0xff)) {
		nextbit += 0x08;
		value >>= 8;
	}

	if (!(value & 0xf)) {
		nextbit += 0x04;
		value >>= 4;
	}

	nextbit += irq_bits[value & 0x7];
	if (nextbit > 63) {
		panic("nextbit out of range: %d\n", nextbit);
	}
	return nextbit;
}

/*
 * ubicom32_reserve_handler()
 *	Bogus handler associated with pre-reserved IRQ(s).
 */
static irqreturn_t ubicom32_reserve_handler(int irq, void *dev_id)
{
	BUG();
	return IRQ_HANDLED;
}

/*
 * __irq_disable_vector()
 *	Disable the interrupt by clearing the appropriate bit in the
 *	LDSR Mask Register.
 */
static void __irq_disable_vector(unsigned int irq)
{
	ldsr_disable_vector(irq);
}

/*
 * __irq_ack_vector()
 *	Acknowledge the specific interrupt by clearing the associate bit in
 *	hardware
 */
static void __irq_ack_vector(unsigned int irq)
{
	if (irq < 32) {
		asm volatile ("move.4 INT_CLR0, %0" : : "d" (1 << irq));
	} else {
		asm volatile ("move.4 INT_CLR1, %0" : : "d" (1 << (irq - 32)));
	}
}

/*
 * __irq_enable_vector()
 *	Clean and then enable the interrupt by setting the appropriate bit in
 *	the LDSR Mask Register.
 */
static void __irq_enable_vector(unsigned int irq)
{
	/*
	 * Acknowledge, really clear the vector.
	 */
	__irq_ack_vector(irq);
	ldsr_enable_vector(irq);
}

/*
 * __irq_mask_vector()
 */
static void __irq_mask_vector(unsigned int irq)
{
	ldsr_mask_vector(irq);
}

/*
 * __irq_unmask_vector()
 */
static void __irq_unmask_vector(unsigned int irq)
{
	ldsr_unmask_vector(irq);
}

/*
 * __irq_end_vector()
 *	Called once an interrupt is completed (reset the LDSR mask).
 */
static void __irq_end_vector(unsigned int irq)
{
	ldsr_unmask_vector(irq);
}

#if defined(CONFIG_SMP)
/*
 * __irq_set_affinity()
 *	Set the cpu affinity for this interrupt.
 *	affinity container allocated at boot
 */
static void __irq_set_affinity(unsigned int irq, const struct cpumask *dest)
{
	smp_set_affinity(irq, dest);
	cpumask_copy(irq_desc[irq].affinity, dest);
}
#endif

/*
 * On-Chip Generic Interrupt function handling.
 */
static struct irq_chip ubicom32_irq_chip = {
	.name		= "Ubicom32",
	.startup	= NULL,
	.shutdown	= NULL,
	.enable		= __irq_enable_vector,
	.disable	= __irq_disable_vector,
	.ack		= __irq_ack_vector,
	.mask		= __irq_mask_vector,
	.unmask		= __irq_unmask_vector,
	.end		= __irq_end_vector,
#if defined(CONFIG_SMP)
	.set_affinity	= __irq_set_affinity,
#endif
};

/*
 * do_IRQ()
 *	Primary interface for handling IRQ() requests.
 */
asmlinkage void do_IRQ(int irq, struct pt_regs *regs)
{
	struct pt_regs *oldregs;
	struct thread_info *ti = current_thread_info();

	IRQ_DECLARE_MEASUREMENT;

	/*
	 * Mark that we are inside of an interrupt and
	 * that interrupts are disabled.
	 */
	oldregs = set_irq_regs(regs);
	ti->interrupt_nesting++;
	trace_hardirqs_off();
	irq_kernel_stack_check(irq, regs);

	/*
	 * Start the interrupt sequence
	 */
	irq_enter();

	/*
	 * Execute the IRQ handler and any pending SoftIRQ requests.
	 */
	BUG_ON(!irqs_disabled());
	IRQ_MEASUREMENT_START();
	__do_IRQ(irq);
	IRQ_MEASUREMENT_END(irq);
	BUG_ON(!irqs_disabled());

	/*
	 * TODO: Since IRQ's are disabled when calling irq_exit()
	 * modify Kconfig to set __ARCH_IRQ_EXIT_IRQS_DISABLED flag.
	 * This will slightly improve performance by enabling
	 * softirq handling to avoid disabling/disabled interrupts.
	 */
	IRQ_MEASUREMENT_START();
	irq_exit();
	IRQ_MEASUREMENT_END(NR_IRQS);
	BUG_ON(!irqs_disabled());

	/*
	 * Outside of an interrupt (or nested exit).
	 */
	set_irq_regs(oldregs);
	trace_hardirqs_on();
	ti->interrupt_nesting--;
}

/*
 * irq_soft_alloc()
 *	Allocate a soft IRQ.
 */
int irq_soft_alloc(unsigned int *soft)
{
	if (irq_soft_avail == 0) {
		printk(KERN_NOTICE "no soft irqs to allocate\n");
		return -EFAULT;
	}

	*soft = irq_get_lsb(irq_soft_avail);
	irq_soft_avail &= ~(1 << *soft);
	return 0;
}

/*
 * ack_bad_irq()
 *	Called to handle an bad irq request.
 */
void ack_bad_irq(unsigned int irq)
{
	printk(KERN_ERR "IRQ: unexpected irq=%d\n", irq);
	__irq_end_vector(irq);
}

/*
 * show_interrupts()
 *	Return a string that displays the state of each of the interrupts.
 */
int show_interrupts(struct seq_file *p, void *v)
{
	struct irqaction *ap;
	int irq = *((loff_t *) v);
	int j;

	if (irq >= NR_IRQS) {
		return 0;
	}

	if (irq == 0) {
		seq_puts(p, "           ");
		for_each_online_cpu(j) {
			seq_printf(p, "CPU%d       ", j);
		}
		seq_putc(p, '\n');
	}

	ap = irq_desc[irq].action;
	if (ap) {
		seq_printf(p, "%3d: ", irq);
		for_each_online_cpu(j) {
			seq_printf(p, "%10u ", kstat_irqs_cpu(irq, j));
		}
		seq_printf(p, "%14s  ", irq_desc[irq].chip->name);
		seq_printf(p, "%s", ap->name);
		for (ap = ap->next; ap; ap = ap->next) {
			seq_printf(p, ", %s", ap->name);
		}
		seq_putc(p, '\n');
	}
	return 0;
}

#if defined(CONFIG_DEBUG_IRQMEASURE)
static unsigned int irq_cycles_to_micro(unsigned int cycles, unsigned int frequency)
{
	unsigned int micro = (cycles / (frequency / 1000000));
	return micro;
}

/*
 * irq_measurement_show()
 *	Print out the min, avg, max values for each IRQ
 *
 * By request, the max value is reset after each dump.
 */
static int irq_measurement_show(struct seq_file *p, void *v)
{
	struct irqaction *ap;
	unsigned int freq = processor_frequency();
	int irq = *((loff_t *) v);


	if (irq == 0) {
		seq_puts(p, "\tmin\tavg\tmax\t(micro-seconds)\n");
	}

	if (irq > NR_IRQS) {
		return 0;
	}

	if (irq == NR_IRQS) {
		unsigned int min, avg, max;
		spin_lock(&irq_measurement_lock);
		min = irq_cycles_to_micro(irq_measurements[irq].min, freq);
		avg = irq_cycles_to_micro(irq_measurements[irq].avg, freq);
		max = irq_cycles_to_micro(irq_measurements[irq].max, freq);
		irq_measurements[irq].max = 0;
		spin_unlock(&irq_measurement_lock);
		seq_printf(p, "   \t%u\t%u\t%u\tsoftirq\n", min, avg, max);
		return 0;
	}

	ap = irq_desc[irq].action;
	if (ap) {
		unsigned int min, avg, max;
		spin_lock(&irq_measurement_lock);
		min = irq_cycles_to_micro(irq_measurements[irq].min, freq);
		avg = irq_cycles_to_micro(irq_measurements[irq].avg, freq);
		max = irq_cycles_to_micro(irq_measurements[irq].max, freq);
		irq_measurements[irq].max = 0;
		spin_unlock(&irq_measurement_lock);
		seq_printf(p, "%2u:\t%u\t%u\t%u\t%s\n", irq, min, avg, max, ap->name);
	}
	return 0;
}

static void *irq_measurement_start(struct seq_file *f, loff_t *pos)
{
	return (*pos <= NR_IRQS) ? pos : NULL;
}

static void *irq_measurement_next(struct seq_file *f, void *v, loff_t *pos)
{
	(*pos)++;
	if (*pos > NR_IRQS)
		return NULL;
	return pos;
}

static void irq_measurement_stop(struct seq_file *f, void *v)
{
	/* Nothing to do */
}

static const struct seq_operations irq_measurement_seq_ops = {
	.start = irq_measurement_start,
	.next  = irq_measurement_next,
	.stop  = irq_measurement_stop,
	.show  = irq_measurement_show,
};

static int irq_measurement_open(struct inode *inode, struct file *filp)
{
	return seq_open(filp, &irq_measurement_seq_ops);
}

static const struct file_operations irq_measurement_fops = {
	.open		= irq_measurement_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

static int __init irq_measurement_init(void)
{
	proc_create("irq_measurements", 0, NULL, &irq_measurement_fops);
	return 0;
}
module_init(irq_measurement_init);
#endif

/*
 * init_IRQ(void)
 *	Initialize the on-chip IRQ subsystem.
 */
void __init init_IRQ(void)
{
	int irq;
	struct devtree_node *p = NULL;
	struct devtree_node *iter = NULL;
	unsigned int mask = 0;
	unsigned int reserved = 0;

	/*
	 * Pull out the list of software interrupts that are avialable to
	 * Linux and provide an allocation function for them.  The first
	 * 24 interrupts of INT0 are software interrupts.
	 */
	irq_soft_avail = 0;
	if (processor_interrupts(&irq_soft_avail, NULL) < 0) {
		printk(KERN_WARNING "No Soft IRQ(s) available\n");
	}
	irq_soft_avail &= ((1 << 24) - 1);

	/*
	 * Initialize all of the on-chip interrupt handling
	 * to use a common set of interrupt functions.
	 */
	for (irq = 0; irq < NR_IRQS; irq++) {
		irq_desc[irq].status = IRQ_DISABLED;
		irq_desc[irq].action = NULL;
		irq_desc[irq].depth = 1;
		set_irq_chip(irq, &ubicom32_irq_chip);
	}

	/*
	 * The sendirq of a devnode is not registered within Linux but instead
	 * is used by the software I/O thread.  These interrupts are reserved.
	 * The recvirq is used by Linux and registered by a device driver, these
	 * are not reserved.
	 *
	 * recvirq(s) that are in the software interrupt range are not supposed
	 * to be marked as reserved.  We track this while we scan the device
	 * nodes.
	 */
	p = devtree_find_next(&iter);
	while (p) {
		unsigned char sendirq, recvirq;
		devtree_irq(p, &sendirq, &recvirq);

		/*
		 * If the sendirq is valid, mark that irq as taken by the
		 * devtree node.
		 */
		if (sendirq < NR_IRQS) {
			ubicom32_reserve_action[sendirq].handler =
				ubicom32_reserve_handler;
			ubicom32_reserve_action[sendirq].name = p->name;
			irq_desc[sendirq].action =
				&ubicom32_reserve_action[sendirq];
			mask |= (1 << sendirq);
		}

		/*
		 * Track the relevant recieve IRQ(s)
		 */
		if (recvirq < 24) {
			mask |= (1 << recvirq);
		}

		/*
		 * Move to the next node.
		 */
		p = devtree_find_next(&iter);
	}

	/*
	 * Remove these bits from the irq_soft_avail list and then use the
	 * result as the list of pre-reserved IRQ(s).
	 */
	reserved = ~irq_soft_avail & ~mask;
	for (irq = 0; irq < 24; irq++) {
		if ((reserved & (1 << irq))) {
			ubicom32_reserve_action[irq].handler =
				ubicom32_reserve_handler;
			ubicom32_reserve_action[irq].name = "reserved";
			irq_desc[irq].action = &ubicom32_reserve_action[irq];
		}
	}

	/*
	 * Initialize the LDSR which is the Ubicom32 programmable
	 * interrupt controller.
	 */
	ldsr_init();

	/*
	 * The Ubicom trap code needs a 2nd init after IRQ(s) are setup.
	 */
	trap_init_interrupt();
}
