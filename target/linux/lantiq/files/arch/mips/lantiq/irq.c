/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/module.h>
#include <linux/interrupt.h>

#include <asm/bootinfo.h>
#include <asm/irq_cpu.h>

#include <lantiq.h>
#include <irq.h>

#define LQ_ICU_BASE_ADDR	(KSEG1 | 0x1F880200)

#define LQ_ICU_IM0_ISR		((u32 *)(LQ_ICU_BASE_ADDR + 0x0000))
#define LQ_ICU_IM0_IER		((u32 *)(LQ_ICU_BASE_ADDR + 0x0008))
#define LQ_ICU_IM0_IOSR		((u32 *)(LQ_ICU_BASE_ADDR + 0x0010))
#define LQ_ICU_IM0_IRSR		((u32 *)(LQ_ICU_BASE_ADDR + 0x0018))
#define LQ_ICU_IM0_IMR		((u32 *)(LQ_ICU_BASE_ADDR + 0x0020))

#define LQ_ICU_IM1_ISR		((u32 *)(LQ_ICU_BASE_ADDR + 0x0028))
#define LQ_ICU_IM2_ISR		((u32 *)(LQ_ICU_BASE_ADDR + 0x0050))
#define LQ_ICU_IM3_ISR		((u32 *)(LQ_ICU_BASE_ADDR + 0x0078))
#define LQ_ICU_IM4_ISR		((u32 *)(LQ_ICU_BASE_ADDR + 0x00A0))

#define LQ_ICU_OFFSET		(LQ_ICU_IM1_ISR - LQ_ICU_IM0_ISR)

void
lq_disable_irq(unsigned int irq_nr)
{
	u32 *ier = LQ_ICU_IM0_IER;
	irq_nr -= INT_NUM_IRQ0;
	ier += LQ_ICU_OFFSET * (irq_nr / INT_NUM_IM_OFFSET);
	irq_nr %= INT_NUM_IM_OFFSET;
	lq_w32(lq_r32(ier) & ~(1 << irq_nr), ier);
}
EXPORT_SYMBOL(lq_disable_irq);

void
lq_mask_and_ack_irq(unsigned int irq_nr)
{
	u32 *ier = LQ_ICU_IM0_IER;
	u32 *isr = LQ_ICU_IM0_ISR;
	irq_nr -= INT_NUM_IRQ0;
	ier += LQ_ICU_OFFSET * (irq_nr / INT_NUM_IM_OFFSET);
	isr += LQ_ICU_OFFSET * (irq_nr / INT_NUM_IM_OFFSET);
	irq_nr %= INT_NUM_IM_OFFSET;
	lq_w32(lq_r32(ier) & ~(1 << irq_nr), ier);
	lq_w32((1 << irq_nr), isr);
}
EXPORT_SYMBOL(lq_mask_and_ack_irq);

static void
lq_ack_irq(unsigned int irq_nr)
{
	u32 *isr = LQ_ICU_IM0_ISR;
	irq_nr -= INT_NUM_IRQ0;
	isr += LQ_ICU_OFFSET * (irq_nr / INT_NUM_IM_OFFSET);
	irq_nr %= INT_NUM_IM_OFFSET;
	lq_w32((1 << irq_nr), isr);
}

void
lq_enable_irq(unsigned int irq_nr)
{
	u32 *ier = LQ_ICU_IM0_IER;
	irq_nr -= INT_NUM_IRQ0;
	ier += LQ_ICU_OFFSET  * (irq_nr / INT_NUM_IM_OFFSET);
	irq_nr %= INT_NUM_IM_OFFSET;
	lq_w32(lq_r32(ier) | (1 << irq_nr), ier);
}
EXPORT_SYMBOL(lq_enable_irq);

static unsigned int
lq_startup_irq(unsigned int irq)
{
	lq_enable_irq(irq);
	return 0;
}

static void
lq_end_irq(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		lq_enable_irq(irq);
}

static struct irq_chip
lq_irq_type = {
	"lq_irq",
	.startup = lq_startup_irq,
	.enable = lq_enable_irq,
	.disable = lq_disable_irq,
	.unmask = lq_enable_irq,
	.ack = lq_ack_irq,
	.mask = lq_disable_irq,
	.mask_ack = lq_mask_and_ack_irq,
	.end = lq_end_irq,
};

static void
lq_hw_irqdispatch(int module)
{
	u32 irq;

	irq = lq_r32(LQ_ICU_IM0_IOSR + (module * LQ_ICU_OFFSET));
	if (irq == 0)
		return;

	/* silicon bug causes only the msb set to 1 to be valid. all
	   other bits might be bogus */
	irq = __fls(irq);
	do_IRQ((int)irq + INT_NUM_IM0_IRL0 + (INT_NUM_IM_OFFSET * module));
}

#define DEFINE_HWx_IRQDISPATCH(x) \
static void lq_hw ## x ## _irqdispatch(void)\
{\
	lq_hw_irqdispatch(x); \
}
static void lq_hw5_irqdispatch(void)
{
	do_IRQ(MIPS_CPU_TIMER_IRQ);
}
DEFINE_HWx_IRQDISPATCH(0)
DEFINE_HWx_IRQDISPATCH(1)
DEFINE_HWx_IRQDISPATCH(2)
DEFINE_HWx_IRQDISPATCH(3)
DEFINE_HWx_IRQDISPATCH(4)
/*DEFINE_HWx_IRQDISPATCH(5)*/

asmlinkage void
plat_irq_dispatch(void)
{
	unsigned int pending = read_c0_status() & read_c0_cause() & ST0_IM;
	unsigned int i;

	if (pending & CAUSEF_IP7)
	{
		do_IRQ(MIPS_CPU_TIMER_IRQ);
		goto out;
	} else {
		for (i = 0; i < 5; i++)
		{
			if (pending & (CAUSEF_IP2 << i))
			{
				lq_hw_irqdispatch(i);
				goto out;
			}
		}
	}
	printk(KERN_ALERT "Spurious IRQ: CAUSE=0x%08x\n", read_c0_status());

out:
	return;
}

static struct irqaction
cascade = {
	.handler = no_action,
	.flags = IRQF_DISABLED,
	.name = "cascade",
};

void __init
arch_init_irq(void)
{
	int i;

	for (i = 0; i < 5; i++)
		lq_w32(0, LQ_ICU_IM0_IER + (i * LQ_ICU_OFFSET));

	mips_cpu_irq_init();

	for (i = 2; i <= 6; i++)
		setup_irq(i, &cascade);

	if (cpu_has_vint) {
		printk(KERN_INFO "Setting up vectored interrupts\n");
		set_vi_handler(2, lq_hw0_irqdispatch);
		set_vi_handler(3, lq_hw1_irqdispatch);
		set_vi_handler(4, lq_hw2_irqdispatch);
		set_vi_handler(5, lq_hw3_irqdispatch);
		set_vi_handler(6, lq_hw4_irqdispatch);
		set_vi_handler(7, lq_hw5_irqdispatch);
	}

	for (i = INT_NUM_IRQ0; i <= (INT_NUM_IRQ0 + (5 * INT_NUM_IM_OFFSET)); i++)
		set_irq_chip_and_handler(i, &lq_irq_type,
			handle_level_irq);

	#if !defined(CONFIG_MIPS_MT_SMP) && !defined(CONFIG_MIPS_MT_SMTC)
	set_c0_status(IE_IRQ0 | IE_IRQ1 | IE_IRQ2 |
		IE_IRQ3 | IE_IRQ4 | IE_IRQ5);
	#else
	set_c0_status(IE_SW0 | IE_SW1 | IE_IRQ0 | IE_IRQ1 |
		IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5);
	#endif
}

void __cpuinit
arch_fixup_c0_irqs(void)
{
	/* FIXME: check for CPUID and only do fix for specific chips/versions */
	cp0_compare_irq = CP0_LEGACY_COMPARE_IRQ;
	cp0_perfcount_irq = CP0_LEGACY_PERFCNT_IRQ;
}

