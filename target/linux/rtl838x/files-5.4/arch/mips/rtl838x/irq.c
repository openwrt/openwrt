// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek RTL838X architecture specific IRQ handling
 *
 * Copyright  (C) 2020 B. Koblitz
 * based on the original BSP
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irqchip.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/spinlock.h>

#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>
#include <mach-rtl838x.h>

extern struct rtl838x_soc_info soc_info;

#define icu_r32(reg)			rtl838x_r32(soc_info.icu_base + reg)
#define icu_w32(val, reg)		rtl838x_w32(val, soc_info.icu_base + reg)
#define icu_w32_mask(clear, set, reg)	rtl838x_w32_mask(clear, set, soc_info.icu_base + reg)

static DEFINE_RAW_SPINLOCK(irq_lock);

extern irqreturn_t c0_compare_interrupt(int irq, void *dev_id);
unsigned int rtl838x_ictl_irq_dispatch1(void);
unsigned int rtl838x_ictl_irq_dispatch2(void);
unsigned int rtl838x_ictl_irq_dispatch3(void);
unsigned int rtl838x_ictl_irq_dispatch4(void);
unsigned int rtl838x_ictl_irq_dispatch5(void);

static struct irqaction irq_cascade1 = {
	.handler = no_action,
	.name = "RTL838X IRQ cascade1",
};

static struct irqaction irq_cascade2 = {
	.handler = no_action,
	.name = "RTL838X IRQ cascade2",
};

static struct irqaction irq_cascade3 = {
	.handler = no_action,
	.name = "RTL838X IRQ cascade3",
};

static struct irqaction irq_cascade4 = {
	.handler = no_action,
	.name = "RTL838X IRQ cascade4",
};

static struct irqaction irq_cascade5 = {
	.handler = no_action,
	.name = "RTL838X IRQ cascade5",
};

static void rtl838x_ictl_enable_irq(struct irq_data *i)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&irq_lock, flags);
	icu_w32_mask(0, 1 << i->irq, GIMR);
	raw_spin_unlock_irqrestore(&irq_lock, flags);
}

static unsigned int rtl838x_ictl_startup_irq(struct irq_data *i)
{
	rtl838x_ictl_enable_irq(i);
	return 0;
}

static void rtl838x_ictl_disable_irq(struct irq_data *i)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&irq_lock, flags);
	icu_w32_mask(1 << i->irq, 0, GIMR);
	raw_spin_unlock_irqrestore(&irq_lock, flags);
}

static void rtl838x_ictl_eoi_irq(struct irq_data *i)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&irq_lock, flags);
	icu_w32_mask(0, 1 << i->irq, GIMR);
	raw_spin_unlock_irqrestore(&irq_lock, flags);
}

static struct irq_chip rtl838x_ictl_irq = {
	.name = "RTL838X",
	.irq_startup = rtl838x_ictl_startup_irq,
	.irq_shutdown = rtl838x_ictl_disable_irq,
	.irq_enable = rtl838x_ictl_enable_irq,
	.irq_disable = rtl838x_ictl_disable_irq,
	.irq_ack = rtl838x_ictl_disable_irq,
	.irq_mask = rtl838x_ictl_disable_irq,
	.irq_unmask = rtl838x_ictl_enable_irq,
	.irq_eoi = rtl838x_ictl_eoi_irq,
};

/*
 *   RTL8390/80/28 Interrupt Scheme
 *
 *   Source       IRQ      CPU INT
 *   --------   -------    -------
 *   UART0          31        IP3
 *   UART1          30        IP2
 *   TIMER0         29        IP6
 *   TIMER1         28        IP2
 *   OCPTO          27        IP2
 *   HLXTO          26        IP2
 *   SLXTO          25        IP2
 *   NIC            24        IP5
 *   GPIO_ABCD      23        IP5
 *   SWCORE         20        IP4
 */

unsigned int rtl838x_ictl_irq_dispatch1(void)
{
	/* Identify shared IRQ  */
	unsigned int extint_ip = icu_r32(GIMR) & icu_r32(GISR);

	if (extint_ip & TC1_IP)
		do_IRQ(TC1_IRQ);
	else if (extint_ip & UART1_IP)
		do_IRQ(UART1_IRQ);
	else
		spurious_interrupt();

	return IRQ_HANDLED;
}

unsigned int rtl838x_ictl_irq_dispatch2(void)
{
	do_IRQ(UART0_IRQ);
	return IRQ_HANDLED;
}

unsigned int rtl838x_ictl_irq_dispatch3(void)
{
	do_IRQ(SWCORE_IRQ);
	return IRQ_HANDLED;
}

unsigned int rtl838x_ictl_irq_dispatch4(void)
{
	/* Identify shared IRQ */
	unsigned int extint_ip = icu_r32(GIMR) & icu_r32(GISR);

	if (extint_ip & NIC_IP)
		do_IRQ(NIC_IRQ);
	else if (extint_ip & GPIO_ABCD_IP)
		do_IRQ(GPIO_ABCD_IRQ);
	else if ((extint_ip & GPIO_EFGH_IP) && (soc_info.family == RTL8328_FAMILY_ID))
		do_IRQ(GPIO_EFGH_IRQ);
	else
		spurious_interrupt();

	return IRQ_HANDLED;
}

unsigned int rtl838x_ictl_irq_dispatch5(void)
{
	do_IRQ(TC0_IRQ);
	return IRQ_HANDLED;
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned int pending;

	pending =  read_c0_cause() & read_c0_status() & ST0_IM;

	if (pending & CAUSEF_IP7)
		c0_compare_interrupt(7, NULL);
	else if (pending & CAUSEF_IP6)
		rtl838x_ictl_irq_dispatch5();
	else if (pending & CAUSEF_IP5)
		rtl838x_ictl_irq_dispatch4();
	else if (pending & CAUSEF_IP4)
		rtl838x_ictl_irq_dispatch3();
	else if (pending & CAUSEF_IP3)
		rtl838x_ictl_irq_dispatch2();
	else if (pending & CAUSEF_IP2)
		rtl838x_ictl_irq_dispatch1();
	else
		spurious_interrupt();
}

static void __init rtl838x_ictl_irq_init(unsigned int irq_base)
{
	int i;

	for (i = 0; i < RTL838X_IRQ_ICTL_NUM; i++)
		irq_set_chip_and_handler(irq_base + i, &rtl838x_ictl_irq, handle_level_irq);

	setup_irq(RTL838X_ICTL1_IRQ, &irq_cascade1);
	setup_irq(RTL838X_ICTL2_IRQ, &irq_cascade2);
	setup_irq(RTL838X_ICTL3_IRQ, &irq_cascade3);
	setup_irq(RTL838X_ICTL4_IRQ, &irq_cascade4);
	setup_irq(RTL838X_ICTL5_IRQ, &irq_cascade5);

	/* Set GIMR, IRR */
	icu_w32(TC0_IE | UART0_IE, GIMR);
	icu_w32(IRR0_SETTING, IRR0);
	icu_w32(IRR1_SETTING, IRR1);
	icu_w32(IRR2_SETTING, IRR2);
	icu_w32(IRR3_SETTING, IRR3);
}

static int intc_map(struct irq_domain *d, unsigned int irq, irq_hw_number_t hw)
{
	irq_set_chip_and_handler(hw, &rtl838x_ictl_irq, handle_level_irq);

	return 0;
}

static const struct irq_domain_ops irq_domain_ops = {
	.xlate = irq_domain_xlate_onecell,
	.map = intc_map,
};

int __init icu_of_init(struct device_node *node, struct device_node *parent)
{
	int i;
	struct irq_domain *domain;
	struct resource res;

	pr_info("Found Interrupt controller: %s (%s)\n", node->name, node->full_name);
	if (of_address_to_resource(node, 0, &res)) {
		panic("Failed to get icu memory range");
	}
	if (!request_mem_region(res.start, resource_size(&res), res.name))
		pr_err("Failed to request icu memory\n");
	soc_info.icu_base = ioremap(res.start, resource_size(&res));
	pr_info("ICU Memory: %08x\n", (u32)soc_info.icu_base);

	mips_cpu_irq_init();

	domain = irq_domain_add_simple(node, 32, 0, &irq_domain_ops, NULL);

	/* Setup all external HW irqs */
	for (i = 8; i < 32; i++)
		irq_domain_associate(domain, i, i);

	rtl838x_ictl_irq_init(RTL838X_IRQ_ICTL_BASE);
	return 0;
}

void __init arch_init_irq(void)
{
	/* do board-specific irq initialization */
	irqchip_init();
}

IRQCHIP_DECLARE(mips_cpu_intc, "rtl838x,icu", icu_of_init);
