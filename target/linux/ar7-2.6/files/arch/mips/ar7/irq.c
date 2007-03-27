/*
 * $Id$
 * 
 * Copyright (C) 2006, 2007 OpenWrt.org
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>

#include <asm/irq.h>
#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>
#include <asm/ar7/ar7.h>

#define EXCEPT_OFFSET 0x80
#define PACE_OFFSET   0xA0
#define CHNLS_OFFSET  0x200

#define REG_OFFSET(irq, reg) ((irq) / 32 * 0x4 + reg * 0x10)
#define SEC_REG_OFFSET(reg) (EXCEPT_OFFSET + reg * 0x8)
#define SR_OFFSET  (SEC_REG_OFFSET(0))
#define CR_OFFSET(irq)  (REG_OFFSET(irq, 1))
#define SEC_CR_OFFSET  (SEC_REG_OFFSET(1))
#define ESR_OFFSET(irq) (REG_OFFSET(irq, 2))
#define SEC_ESR_OFFSET  (SEC_REG_OFFSET(2))
#define ECR_OFFSET(irq) (REG_OFFSET(irq, 3))
#define SEC_ECR_OFFSET  (SEC_REG_OFFSET(3))
#define PIR_OFFSET      (0x40)
#define MSR_OFFSET      (0x44)
#define PM_OFFSET(irq)  (REG_OFFSET(irq, 5))
#define TM_OFFSET(irq)  (REG_OFFSET(irq, 6))

#define REG(addr) (*(volatile u32 *)(KSEG1ADDR(AR7_REGS_IRQ) + addr))

#define CHNL_OFFSET(chnl) (CHNLS_OFFSET + (chnl * 4))

static void ar7_unmask_irq(unsigned int irq_nr);
static void ar7_mask_irq(unsigned int irq_nr);
static void ar7_unmask_secondary_irq(unsigned int irq_nr);
static void ar7_mask_secondary_irq(unsigned int irq_nr);
static irqreturn_t ar7_cascade(int interrupt, void *dev);
static irqreturn_t ar7_secondary_cascade(int interrupt, void *dev);
static void ar7_irq_init(int base);
static int ar7_irq_base;

static struct irq_chip ar7_irq_type = {
	.name = "AR7",
	.unmask = ar7_unmask_irq,
	.mask = ar7_mask_irq,
};

static struct irq_chip ar7_secondary_irq_type = {
	.name = "AR7",
	.unmask = ar7_unmask_secondary_irq,
	.mask = ar7_mask_secondary_irq,
};

static struct irqaction ar7_cascade_action = {
	.handler = ar7_cascade, 
	.name = "AR7 cascade interrupt"
};

static struct irqaction ar7_secondary_cascade_action = {
	.handler = ar7_secondary_cascade, 
	.name = "AR7 secondary cascade interrupt"
};

static void ar7_unmask_irq(unsigned int irq)
{
	unsigned long flags;
	local_irq_save(flags);
	/* enable the interrupt channel  bit */
	REG(ESR_OFFSET(irq)) = 1 << ((irq - ar7_irq_base) % 32);
	local_irq_restore(flags);
}

static void ar7_mask_irq(unsigned int irq)
{
	unsigned long flags;
	local_irq_save(flags);
	/* disable the interrupt channel bit */
	REG(ECR_OFFSET(irq)) = 1 << ((irq - ar7_irq_base) % 32);
	local_irq_restore(flags);
}

static void ar7_unmask_secondary_irq(unsigned int irq)
{
	unsigned long flags;
	local_irq_save(flags);
	/* enable the interrupt channel  bit */
	REG(SEC_ESR_OFFSET) = 1 << (irq - ar7_irq_base - 40);
	local_irq_restore(flags);
}

static void ar7_mask_secondary_irq(unsigned int irq)
{
	unsigned long flags;
	local_irq_save(flags);
	/* disable the interrupt channel bit */
	REG(SEC_ECR_OFFSET) = 1 << (irq - ar7_irq_base - 40);
	local_irq_restore(flags);
}

void __init arch_init_irq(void) {
	mips_cpu_irq_init(0);
	ar7_irq_init(8);
}

static void __init ar7_irq_init(int base)
{
	int i;
	/*  
	    Disable interrupts and clear pending
	*/
	REG(ECR_OFFSET(0)) = 0xffffffff;
	REG(ECR_OFFSET(32)) = 0xff;
	REG(SEC_ECR_OFFSET) = 0xffffffff;
	REG(CR_OFFSET(0)) = 0xffffffff;
	REG(CR_OFFSET(32)) = 0xff;
	REG(SEC_CR_OFFSET) = 0xffffffff;

	ar7_irq_base = base;

	for(i = 0; i < 40; i++) {
		REG(CHNL_OFFSET(i)) = i;
		/* Primary IRQ's */
		irq_desc[i + base].status = IRQ_DISABLED;
		irq_desc[i + base].action = NULL;
		irq_desc[i + base].depth = 1;
		irq_desc[i + base].chip = &ar7_irq_type;
		/* Secondary IRQ's */
		if (i < 32) {
			irq_desc[i + base + 40].status = IRQ_DISABLED;
			irq_desc[i + base + 40].action = NULL;
			irq_desc[i + base + 40].depth = 1;
			irq_desc[i + base + 40].chip = &ar7_secondary_irq_type;
		}
	}

	setup_irq(2, &ar7_cascade_action);
	setup_irq(ar7_irq_base, &ar7_secondary_cascade_action);
	set_c0_status(IE_IRQ0);
}

static irqreturn_t ar7_cascade(int interrupt, void *dev)
{
	int irq;

	irq = (REG(PIR_OFFSET) & 0x3F);
	REG(CR_OFFSET(irq)) = 1 << (irq % 32);

	do_IRQ(irq + ar7_irq_base);

	return IRQ_HANDLED;
}

static irqreturn_t ar7_secondary_cascade(int interrupt, void *dev)
{
	int irq = 0, i;
	unsigned long status;

	status = REG(SR_OFFSET);
	if (unlikely(!status)) {
		spurious_interrupt();
		return IRQ_NONE;
	}

	for (i = 0; i < 32; i++)
		if (status & (i << 1)) {
			irq = i + 40;
			REG(SEC_CR_OFFSET) = 1 << i;
			break;
		}

	do_IRQ(irq + ar7_irq_base);

	return IRQ_HANDLED;
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned int pending = read_c0_status() & read_c0_cause();
	if (pending & STATUSF_IP7)		/* cpu timer */
		do_IRQ(7);
	else if (pending & STATUSF_IP2)		/* int0 hardware line */
		do_IRQ(2);
	else 
		spurious_interrupt();
}
