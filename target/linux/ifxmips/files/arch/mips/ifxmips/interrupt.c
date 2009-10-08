/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2005 Wu Qi Ming infineon
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/module.h>

#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_irq.h>
#include <asm/irq_cpu.h>

void ifxmips_disable_irq(unsigned int irq_nr)
{
	int i;
	u32 *ifxmips_ier = IFXMIPS_ICU_IM0_IER;

	irq_nr -= INT_NUM_IRQ0;
	for (i = 0; i <= 4; i++) {
		if (irq_nr < INT_NUM_IM_OFFSET) {
			ifxmips_w32(ifxmips_r32(ifxmips_ier) & ~(1 << irq_nr),
				ifxmips_ier);
			return;
		}
		ifxmips_ier += IFXMIPS_ICU_OFFSET;
		irq_nr -= INT_NUM_IM_OFFSET;
	}
}
EXPORT_SYMBOL(ifxmips_disable_irq);

void ifxmips_mask_and_ack_irq(unsigned int irq_nr)
{
	int i;
	u32 *ifxmips_ier = IFXMIPS_ICU_IM0_IER;
	u32 *ifxmips_isr = IFXMIPS_ICU_IM0_ISR;

	irq_nr -= INT_NUM_IRQ0;
	for (i = 0; i <= 4; i++) {
		if (irq_nr < INT_NUM_IM_OFFSET) {
			ifxmips_w32(ifxmips_r32(ifxmips_ier) & ~(1 << irq_nr),
				ifxmips_ier);
			ifxmips_w32((1 << irq_nr), ifxmips_isr);
			return;
		}
		ifxmips_ier += IFXMIPS_ICU_OFFSET;
		ifxmips_isr += IFXMIPS_ICU_OFFSET;
		irq_nr -= INT_NUM_IM_OFFSET;
	}
}
EXPORT_SYMBOL(ifxmips_mask_and_ack_irq);

void ifxmips_enable_irq(unsigned int irq_nr)
{
	int i;
	u32 *ifxmips_ier = IFXMIPS_ICU_IM0_IER;

	irq_nr -= INT_NUM_IRQ0;
	for (i = 0; i <= 4; i++) {
		if (irq_nr < INT_NUM_IM_OFFSET) {
			ifxmips_w32(ifxmips_r32(ifxmips_ier) | (1 << irq_nr),
				ifxmips_ier);
			return;
		}
		ifxmips_ier += IFXMIPS_ICU_OFFSET;
		irq_nr -= INT_NUM_IM_OFFSET;
	}
}
EXPORT_SYMBOL(ifxmips_enable_irq);

static unsigned int ifxmips_startup_irq(unsigned int irq)
{
	ifxmips_enable_irq(irq);
	return 0;
}

static void ifxmips_end_irq(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ifxmips_enable_irq(irq);
}

static struct hw_interrupt_type ifxmips_irq_type = {
	"IFXMIPS",
	.startup = ifxmips_startup_irq,
	.enable = ifxmips_enable_irq,
	.disable = ifxmips_disable_irq,
	.unmask = ifxmips_enable_irq,
	.ack = ifxmips_end_irq,
	.mask = ifxmips_disable_irq,
	.mask_ack = ifxmips_mask_and_ack_irq,
	.end = ifxmips_end_irq,
};

/* silicon bug causes only the msb set to 1 to be valid. all
   other bits might be bogus */
static inline int ls1bit32(unsigned long x)
{
	__asm__ (
		".set push \n"
		".set mips32 \n"
		"clz %0, %1 \n"
		".set pop \n"
		: "=r" (x)
		: "r" (x));
	return 31 - x;
}

void ifxmips_hw_irqdispatch(int module)
{
	u32 irq;

	irq = ifxmips_r32(IFXMIPS_ICU_IM0_IOSR + (module * IFXMIPS_ICU_OFFSET));
	if (irq == 0)
		return;

	/* we need to do this due to a silicon bug */
	irq = ls1bit32(irq);
	do_IRQ((int)irq + INT_NUM_IM0_IRL0 + (INT_NUM_IM_OFFSET * module));

	if ((irq == 22) && (module == 0))
		ifxmips_w32(ifxmips_r32(IFXMIPS_EBU_PCC_ISTAT) | 0x10,
			IFXMIPS_EBU_PCC_ISTAT);
}

#ifdef CONFIG_CPU_MIPSR2_IRQ_VI
#define DEFINE_HWx_IRQDISPATCH(x) \
static void ifxmips_hw ## x ## _irqdispatch(void)\
{\
	ifxmips_hw_irqdispatch(x); \
}
static void ifxmips_hw5_irqdispatch(void)
{
	do_IRQ(MIPS_CPU_TIMER_IRQ);
}
DEFINE_HWx_IRQDISPATCH(0)
DEFINE_HWx_IRQDISPATCH(1)
DEFINE_HWx_IRQDISPATCH(2)
DEFINE_HWx_IRQDISPATCH(3)
DEFINE_HWx_IRQDISPATCH(4)
/*DEFINE_HWx_IRQDISPATCH(5)*/
#endif /* #ifdef CONFIG_CPU_MIPSR2_IRQ_VI */

asmlinkage void plat_irq_dispatch(void)
{
	unsigned int pending = read_c0_status() & read_c0_cause() & ST0_IM;
	unsigned int i;

	if (pending & CAUSEF_IP7) {
		do_IRQ(MIPS_CPU_TIMER_IRQ);
		goto out;
	} else {
		for (i = 0; i < 5; i++) {
			if (pending & (CAUSEF_IP2 << i)) {
				ifxmips_hw_irqdispatch(i);
				goto out;
			}
		}
	}
	printk(KERN_ALERT "Spurious IRQ: CAUSE=0x%08x\n", read_c0_status());

out:
	return;
}

static struct irqaction cascade = {
	.handler = no_action,
	.flags = IRQF_DISABLED,
	.name = "cascade",
};

void __init arch_init_irq(void)
{
	int i;

	for (i = 0; i < 5; i++)
		ifxmips_w32(0, IFXMIPS_ICU_IM0_IER + (i * IFXMIPS_ICU_OFFSET));

	mips_cpu_irq_init();

	for (i = 2; i <= 6; i++)
		setup_irq(i, &cascade);

#ifdef CONFIG_CPU_MIPSR2_IRQ_VI
	if (cpu_has_vint) {
		printk(KERN_INFO "Setting up vectored interrupts\n");
		set_vi_handler(2, ifxmips_hw0_irqdispatch);
		set_vi_handler(3, ifxmips_hw1_irqdispatch);
		set_vi_handler(4, ifxmips_hw2_irqdispatch);
		set_vi_handler(5, ifxmips_hw3_irqdispatch);
		set_vi_handler(6, ifxmips_hw4_irqdispatch);
		set_vi_handler(7, ifxmips_hw5_irqdispatch);
	}
#endif /* CONFIG_CPU_MIPSR2_IRQ_VI */

	for (i = INT_NUM_IRQ0; i <= (INT_NUM_IRQ0 + (5 * INT_NUM_IM_OFFSET));
		i++)
		set_irq_chip_and_handler(i, &ifxmips_irq_type,
			handle_level_irq);

	#if !defined(CONFIG_MIPS_MT_SMP) && !defined(CONFIG_MIPS_MT_SMTC)
	set_c0_status(IE_IRQ0 | IE_IRQ1 | IE_IRQ2 |
		IE_IRQ3 | IE_IRQ4 | IE_IRQ5);
	#else
	set_c0_status(IE_SW0 | IE_SW1 | IE_IRQ0 | IE_IRQ1 |
		IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5);
	#endif
}

void __cpuinit arch_fixup_c0_irqs(void)
{
	/* FIXME: check for CPUID and only do fix for specific chips/versions */
	cp0_compare_irq = CP0_LEGACY_COMPARE_IRQ;
	cp0_perfcount_irq = CP0_LEGACY_PERFCNT_IRQ;
}
