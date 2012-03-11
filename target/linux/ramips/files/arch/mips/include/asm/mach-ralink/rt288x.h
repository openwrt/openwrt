/*
 * Ralink RT288x SoC specific definitions
 *
 * Copyright (C) 2008-2011 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 * Parts of this file are based on Ralink's 2.6.21 BSP
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RT288X_H_
#define _RT288X_H_

#include <linux/init.h>
#include <linux/io.h>

#define RT288X_MEM_SIZE_MIN    (2 * 1024 * 1024)
#define RT288X_MEM_SIZE_MAX    (128 * 1024 * 1024)

#define RT288X_CPU_IRQ_BASE	0
#define RT288X_INTC_IRQ_BASE	8
#define RT288X_INTC_IRQ_COUNT	32
#define RT288X_GPIO_IRQ_BASE	40

#define RT288X_CPU_IRQ_INTC	(RT288X_CPU_IRQ_BASE + 2)
#define RT288X_CPU_IRQ_PCI	(RT288X_CPU_IRQ_BASE + 4)
#define RT288X_CPU_IRQ_FE	(RT288X_CPU_IRQ_BASE + 5)
#define RT288X_CPU_IRQ_WNIC	(RT288X_CPU_IRQ_BASE + 6)
#define RT288X_CPU_IRQ_COUNTER	(RT288X_CPU_IRQ_BASE + 7)

#define RT2880_INTC_IRQ_TIMER0	(RT288X_INTC_IRQ_BASE + 0)
#define RT2880_INTC_IRQ_TIMER1	(RT288X_INTC_IRQ_BASE + 1)
#define RT2880_INTC_IRQ_UART0	(RT288X_INTC_IRQ_BASE + 2)
#define RT2880_INTC_IRQ_PIO	(RT288X_INTC_IRQ_BASE + 3)
#define RT2880_INTC_IRQ_PCM	(RT288X_INTC_IRQ_BASE + 4)
#define RT2880_INTC_IRQ_UART1	(RT288X_INTC_IRQ_BASE + 8)
#define RT2880_INTC_IRQ_IA	(RT288X_INTC_IRQ_BASE + 23)

#define RT288X_GPIO_IRQ(x)	(RT288X_GPIO_IRQ_BASE + (x))
#define RT288X_GPIO_COUNT	32

extern void __iomem *rt288x_sysc_base;
extern void __iomem *rt288x_memc_base;

static inline void rt288x_sysc_wr(u32 val, unsigned reg)
{
	__raw_writel(val, rt288x_sysc_base + reg);
}

static inline u32 rt288x_sysc_rr(unsigned reg)
{
	return __raw_readl(rt288x_sysc_base + reg);
}

static inline void rt288x_memc_wr(u32 val, unsigned reg)
{
	__raw_writel(val, rt288x_memc_base + reg);
}

static inline u32 rt288x_memc_rr(unsigned reg)
{
	return __raw_readl(rt288x_memc_base + reg);
}

void rt288x_gpio_init(u32 mode);

#ifdef CONFIG_PCI
int rt288x_register_pci(void);
#else
static inline int rt288x_register_pci(void) { return 0; }
#endif /* CONFIG_PCI */

#endif /* _RT228X_H_ */
