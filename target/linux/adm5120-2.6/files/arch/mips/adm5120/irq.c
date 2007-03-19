/*
 *	Copyright (C) ADMtek Incorporated.
 *		Creator : daniell@admtek.com.tw
 *	Carsten Langgaard, carstenl@mips.com
 *	Copyright (C) 2000, 2001 MIPS Technologies, Inc.
 *	Copyright (C) 2001 Ralf Baechle
 *	Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/pm.h>

#include <asm/irq.h>
#include <asm/time.h>
#include <asm/mipsregs.h>
#include <asm/gdb-stub.h>
#include <asm/irq_cpu.h>

#define MIPS_CPU_TIMER_IRQ 7

extern int setup_irq(unsigned int irq, struct irqaction *irqaction);
extern irq_desc_t irq_desc[];
extern asmlinkage void mipsIRQ(void);

int mips_int_lock(void);
void mips_int_unlock(int);

unsigned int mips_counter_frequency;

#define ADM5120_INTC_REG(reg)	(*(volatile u32 *)(KSEG1ADDR(0x12200000+(reg))))
#define ADM5120_INTC_STATUS	ADM5120_INTC_REG(0x00)
#define ADM5120_INTC_ENABLE	ADM5120_INTC_REG(0x08)
#define ADM5120_INTC_DISABLE	ADM5120_INTC_REG(0x0c)
#define ADM5120_IRQ_MAX		9
#define ADM5120_IRQ_MASK	0x3ff

void adm5120_hw0_irqdispatch(struct pt_regs *regs)
{
	unsigned long intsrc;
	int i;

	intsrc = ADM5120_INTC_STATUS & ADM5120_IRQ_MASK;

	for (i = 0; intsrc; intsrc >>= 1, i++)
		if (intsrc & 0x1)
			do_IRQ(i);
		else
			spurious_interrupt();
}

void mips_timer_interrupt(struct pt_regs *regs)
{
        write_c0_compare(read_c0_count()+ mips_counter_frequency/HZ);
        ll_timer_interrupt(MIPS_CPU_TIMER_IRQ);
}

/* Main interrupt dispatcher */
asmlinkage void plat_irq_dispatch(struct pt_regs *regs)
{
        unsigned int cp0_cause = read_c0_cause() & read_c0_status();

        if (cp0_cause & CAUSEF_IP7) {
                mips_timer_interrupt( regs);
        } else if (cp0_cause & CAUSEF_IP2) {
                adm5120_hw0_irqdispatch( regs);
        }
}

void enable_adm5120_irq(unsigned int irq)
{
	int s;

	/* Disable all interrupts (FIQ/IRQ) */
	s = mips_int_lock();

	if ((irq < 0) || (irq > ADM5120_IRQ_MAX)) 
		goto err_exit;

	ADM5120_INTC_ENABLE = (1<<irq);

err_exit:

	/* Restore the interrupts states */
	mips_int_unlock(s);
}


void disable_adm5120_irq(unsigned int irq)
{
	int s;

	/* Disable all interrupts (FIQ/IRQ) */
	s = mips_int_lock();

	if ((irq < 0) || (irq > ADM5120_IRQ_MAX)) 
		goto err_exit;

	ADM5120_INTC_DISABLE = (1<<irq);

err_exit:
	/* Restore the interrupts states */
	mips_int_unlock(s);
}

unsigned int startup_adm5120_irq(unsigned int irq)
{
	enable_adm5120_irq(irq);
	return 0;
}

void shutdown_adm5120_irq(unsigned int irq)
{
	disable_adm5120_irq(irq);
}

static inline void ack_adm5120_irq(unsigned int irq_nr)
{
	ADM5120_INTC_DISABLE = (1 << irq_nr);
}


static void end_adm5120_irq(unsigned int irq_nr)
{
	ADM5120_INTC_ENABLE = (1 << irq_nr);
}

static hw_irq_controller adm5120_irq_type = {
	.typename 	= "MIPS",
	.startup	= startup_adm5120_irq,
	.shutdown	= shutdown_adm5120_irq,
	.enable		= enable_adm5120_irq,
	.disable	= disable_adm5120_irq,
	.ack 		= ack_adm5120_irq,
	.end		= end_adm5120_irq,
	.set_affinity	= NULL,
};


void __init arch_init_irq(void)
{
	int i;
	
	for (i = 0; i <= ADM5120_IRQ_MAX; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = 0;
		irq_desc[i].depth = 1;
		irq_desc[i].chip = &adm5120_irq_type;
	}
}
