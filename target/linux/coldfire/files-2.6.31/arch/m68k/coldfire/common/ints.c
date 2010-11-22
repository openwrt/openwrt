/*
 * linux/arch/m68k/coldfire/ints.c -- General interrupt handling code
 *
 *  Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *  Matt Waddel Matt.Waddel@freescale.com
 *  Kurt Mahan kmahan@freescale.com
 *  Jason Jin Jason.Jin@freescale.com
 *  Shrek Wu B16972@freescale.com
 *
 * Based on:
 * linux/arch/m68k/kernel/ints.c &
 * linux/arch/m68knommu/5307/ints.c
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel_stat.h>
#include <linux/errno.h>
#include <linux/seq_file.h>
#include <linux/interrupt.h>

#include <asm/system.h>
#include <asm/irq.h>
#include <asm/traps.h>
#include <asm/page.h>
#include <asm/machdep.h>
#include <asm/irq_regs.h>

#include <asm/mcfsim.h>

/*
 * IRQ Handler lists.
 */
static struct irq_node *irq_list[SYS_IRQS];
static struct irq_controller *irq_controller[SYS_IRQS];
static int irq_depth[SYS_IRQS];

/*
 * IRQ Controller
 */
#if defined(CONFIG_M5445X)
void m5445x_irq_enable(unsigned int irq);
void m5445x_irq_disable(unsigned int irq);
static struct irq_controller m5445x_irq_controller = {
	.name		= "M5445X",
	.lock		= SPIN_LOCK_UNLOCKED,
	.enable		= m5445x_irq_enable,
	.disable	= m5445x_irq_disable,
};
#elif defined(CONFIG_M547X_8X)
void m547x_8x_irq_enable(unsigned int irq);
void m547x_8x_irq_disable(unsigned int irq);
static struct irq_controller m547x_8x_irq_controller = {
	.name           = "M547X_8X",
	.lock           = SPIN_LOCK_UNLOCKED,
	.enable         = m547x_8x_irq_enable,
	.disable        = m547x_8x_irq_disable,
};
#else
# error No IRQ controller defined
#endif

#define	POOL_SIZE 	SYS_IRQS
static struct irq_node  pool[POOL_SIZE];
static struct irq_node *get_irq_node(void);

/* The number of spurious interrupts */
unsigned int num_spurious;
asmlinkage void handle_badint(struct pt_regs *regs);

/*
 * void init_IRQ(void)
 *
 * This function should be called during kernel startup to initialize
 * the IRQ handling routines.
 */
void __init init_IRQ(void)
{
	int i;

#if defined(CONFIG_M5445X)
	for (i = 0; i < SYS_IRQS; i++)
		irq_controller[i] = &m5445x_irq_controller;
#elif defined(CONFIG_M547X_8X)
	for (i = 0; i < SYS_IRQS; i++)
		irq_controller[i] = &m547x_8x_irq_controller;
#endif
}

/*
 * process_int(unsigned long vec, struct pt_regs *fp)
 *
 * Process an interrupt.  Called from entry.S.
 */
asmlinkage void process_int(unsigned long vec, struct pt_regs *fp)
{
	struct pt_regs *old_regs;
	struct irq_node *node;
	old_regs = set_irq_regs(fp);
	kstat_cpu(0).irqs[vec]++;

	node = irq_list[vec];
	if (!node)
		handle_badint(fp);
	else {
		do {
			node->handler(vec, node->dev_id);
			node = node->next;
		} while (node);
	}

	set_irq_regs(old_regs);
}

/*
 * show_interrupts( struct seq_file *p, void *v)
 *
 * Called to show all the current interrupt information.
 */
int show_interrupts(struct seq_file *p, void *v)
{
	struct irq_controller *contr;
	struct irq_node *node;
	int i = *(loff_t *) v;

	if ((i < NR_IRQS) && (irq_list[i])) {
		contr = irq_controller[i];
		node = irq_list[i];
		seq_printf(p, "%-8s %3u: %10u %s", contr->name, i,
			kstat_cpu(0).irqs[i], node->devname);
		while ((node = node->next))
			seq_printf(p, ", %s", node->devname);

		seq_printf(p, "\n");
	}

	return 0;
}

/*
 * get_irq_node(void)
 *
 * Get an irq node from the pool.
 */
struct irq_node *get_irq_node(void)
{
	struct irq_node *p = pool;
	int i;

	for (i = 0; i < POOL_SIZE; i++, p++) {
		if (!p->handler) {
			memset(p, 0, sizeof(struct irq_node));
			return p;
		}
	}
	printk(KERN_INFO "%s(%s:%d): No more irq nodes, I suggest you \
		increase POOL_SIZE", __FUNCTION__, __FILE__, __LINE__);
	return NULL;
}

void init_irq_proc(void)
{
	/* Insert /proc/irq driver here */
}

int setup_irq(unsigned int irq, struct irq_node *node)
{
	struct irq_controller *contr;
	struct irq_node **prev;
	unsigned long flags;

	if (irq >= NR_IRQS || !irq_controller[irq]) {
		printk("%s: Incorrect IRQ %d from %s\n",
		       __FUNCTION__, irq, node->devname);
		return -ENXIO;
	}

	contr = irq_controller[irq];
	spin_lock_irqsave(&contr->lock, flags);

	prev = irq_list + irq;
	if (*prev) {
		/* Can't share interrupts unless both agree to */
		if (!((*prev)->flags & node->flags & IRQF_SHARED)) {
			spin_unlock_irqrestore(&contr->lock, flags);
			printk(KERN_INFO "%s: -BUSY-Incorrect IRQ %d \n",
				__FUNCTION__, irq);
			return -EBUSY;
		}
		while (*prev)
			prev = &(*prev)->next;
	}

	if (!irq_list[irq]) {
		if (contr->startup)
			contr->startup(irq);
		else
			contr->enable(irq);
	}
	node->next = NULL;
	*prev = node;

	spin_unlock_irqrestore(&contr->lock, flags);

	return 0;
}

int request_irq(unsigned int irq,
		irq_handler_t handler,
		unsigned long flags, const char *devname, void *dev_id)
{
	struct irq_node *node = get_irq_node();
	int res;

	if (!node) {
		printk(KERN_INFO "%s:get_irq_node error %x\n",
			__FUNCTION__,(unsigned int) node);
		return -ENOMEM;
	}
	node->handler = handler;
	node->flags   = flags;
	node->dev_id  = dev_id;
	node->devname = devname;

	res = setup_irq(irq, node);
	if (res)
		node->handler = NULL;

	return res;
}
EXPORT_SYMBOL(request_irq);

void free_irq(unsigned int irq, void *dev_id)
{
	struct irq_controller *contr;
	struct irq_node **p, *node;
	unsigned long flags;

	if (irq >= NR_IRQS || !irq_controller[irq]) {
		printk(KERN_DEBUG "%s: Incorrect IRQ %d\n", __FUNCTION__, irq);
		return;
	}

	contr = irq_controller[irq];
	spin_lock_irqsave(&contr->lock, flags);

	p = irq_list + irq;
	while ((node = *p)) {
		if (node->dev_id == dev_id)
			break;
		p = &node->next;
	}

	if (node) {
		*p = node->next;
		node->handler = NULL;
	} else
		printk(KERN_DEBUG "%s: Removing probably wrong IRQ %d\n",
		       __FUNCTION__, irq);

	if (!irq_list[irq]) {
		if (contr->shutdown)
			contr->shutdown(irq);
		else
			contr->disable(irq);
	}

	spin_unlock_irqrestore(&contr->lock, flags);
}
EXPORT_SYMBOL(free_irq);

void enable_irq(unsigned int irq)
{
	struct irq_controller *contr;
	unsigned long flags;

	if (irq >= NR_IRQS || !irq_controller[irq]) {
		printk(KERN_DEBUG "%s: Incorrect IRQ %d\n", __FUNCTION__, irq);
		return;
	}

	contr = irq_controller[irq];
	spin_lock_irqsave(&contr->lock, flags);
	if (irq_depth[irq]) {
		if (!--irq_depth[irq]) {
			if (contr->enable)
				contr->enable(irq);
		}
	} else
		WARN_ON(1);
	spin_unlock_irqrestore(&contr->lock, flags);
}
EXPORT_SYMBOL(enable_irq);

void disable_irq(unsigned int irq)
{
	struct irq_controller *contr;
	unsigned long flags;

	if (irq >= NR_IRQS || !irq_controller[irq]) {
		printk(KERN_DEBUG "%s: Incorrect IRQ %d\n", __FUNCTION__, irq);
		return;
	}

	contr = irq_controller[irq];
	spin_lock_irqsave(&contr->lock, flags);
	if (!irq_depth[irq]++) {
		if (contr->disable)
			contr->disable(irq);
	}
	spin_unlock_irqrestore(&contr->lock, flags);
}
EXPORT_SYMBOL(disable_irq);

void disable_irq_nosync(unsigned int irq) __attribute__((alias("disable_irq")));
EXPORT_SYMBOL(disable_irq_nosync);


unsigned long probe_irq_on(void)
{
	return 0;
}
EXPORT_SYMBOL(probe_irq_on);

int probe_irq_off(unsigned long irqs)
{
	return 0;
}
EXPORT_SYMBOL(probe_irq_off);

asmlinkage void handle_badint(struct pt_regs *regs)
{
	kstat_cpu(0).irqs[0]++;
	num_spurious++;
	printk(KERN_DEBUG "unexpected interrupt from %u\n", regs->vector);
}
EXPORT_SYMBOL(handle_badint);

unsigned int irq_canonicalize(unsigned int irq)
{                       
#ifdef CONFIG_Q40
        if (MACH_IS_Q40 && irq == 11)
                irq = 10;
#endif
        return irq;
}

EXPORT_SYMBOL(irq_canonicalize);

#ifdef CONFIG_M5445X
/*
 * M5445X Implementation
 */
void m5445x_irq_enable(unsigned int irq)
{
	/* enable the interrupt hardware */
	if (irq < 64)
		return;

	/* adjust past non-hardware ints */
	irq -= 64;

	/* check for eport */
	if ((irq > 0) && (irq < 8)) {
		/* enable eport */
		MCF_EPORT_EPPAR &= ~(3 << (irq*2));	/* level */
		MCF_EPORT_EPDDR &= ~(1 << irq);		/* input */
		MCF_EPORT_EPIER |= 1 << irq;		/* irq enabled */
	}

	if (irq < 64) {
		/* controller 0 */
		MCF_INTC0_ICR(irq) = 0x02;
		MCF_INTC0_CIMR = irq;
	} else {
		/* controller 1 */
		irq -= 64;
		MCF_INTC1_ICR(irq) = 0x02;
		MCF_INTC1_CIMR = irq;
	}
}

void m5445x_irq_disable(unsigned int irq)
{
	/* disable the interrupt hardware */
	if (irq < 64)
		return;

	/* adjust past non-hardware ints */
	irq -= 64;

	/* check for eport */
	if ((irq > 0) && (irq < 8)) {
		/* disable eport */
		MCF_EPORT_EPIER &= ~(1 << irq);
	}

	if (irq < 64) {
		/* controller 0 */
		MCF_INTC0_ICR(irq) = 0x00;
		MCF_INTC0_SIMR = irq;
	} else {
		/* controller 1 */
		irq -= 64;
		MCF_INTC1_ICR(irq) = 0x00;
		MCF_INTC1_SIMR = irq;
	}
}
#elif defined(CONFIG_M547X_8X)
/*
 * M547X_8X Implementation
 */
void m547x_8x_irq_enable(unsigned int irq)
{
	/* enable the interrupt hardware */
	if (irq < 64)
		return;

	/* adjust past non-hardware ints */
	irq -= 64;

	/* check for eport */
	if ((irq > 0) && (irq < 8)) {
		/* enable eport */
		MCF_EPPAR &= ~(3 << (irq*2));
		/* level */
		MCF_EPDDR &= ~(1 << irq);
		/* input */
		MCF_EPIER |= 1 << irq;
		/* irq enabled */
	}

	if (irq < 32) {
		/* *grumble* don't set low bit of IMRL */
		MCF_IMRL &= (~(1 << irq) & 0xfffffffe);
	} else {
		MCF_IMRH &= ~(1 << (irq - 32));
	}
}

void m547x_8x_irq_disable(unsigned int irq)
{
	/* disable the interrupt hardware */
	if (irq < 64)
		return;

	/* adjust past non-hardware ints */
	irq -= 64;

	/* check for eport */
	if ((irq > 0) && (irq < 8)) {
		/* disable eport */
		MCF_EPIER &= ~(1 << irq);
	}

	if (irq < 32)
		MCF_IMRL |= (1 << irq);
	else
		MCF_IMRH |= (1 << (irq - 32));
}
#endif
