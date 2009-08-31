/*
 * Ralink RT305x SoC specific definitions
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * Parts of this file are based on Ralink's 2.6.21 BSP
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RT305X_H_
#define _RT305X_H_

#include <linux/init.h>
#include <linux/io.h>

void rt305x_detect_sys_type(void) __init;
void rt305x_detect_sys_freq(void) __init;

extern unsigned long rt305x_cpu_freq;
extern unsigned long rt305x_sys_freq;

#define RT305X_MEM_SIZE_MIN (2 * 1024 * 1024)
#define RT305X_MEM_SIZE_MAX (64 * 1024 * 1024)

#define RT305X_CPU_IRQ_BASE	0
#define RT305X_INTC_IRQ_BASE	8
#define RT305X_INTC_IRQ_COUNT	32
#define RT305X_GPIO_IRQ_BASE	40

#define RT305X_CPU_IRQ_INTC	(RT305X_CPU_IRQ_BASE + 2)
#define RT305X_CPU_IRQ_FE	(RT305X_CPU_IRQ_BASE + 5)
#define RT305X_CPU_IRQ_WNIC	(RT305X_CPU_IRQ_BASE + 6)
#define RT305X_CPU_IRQ_COUNTER	(RT305X_CPU_IRQ_BASE + 7)

#define RT305X_INTC_IRQ_SYSCTL	(RT305X_INTC_IRQ_BASE + 0)
#define RT305X_INTC_IRQ_TIMER0	(RT305X_INTC_IRQ_BASE + 1)
#define RT305X_INTC_IRQ_TIMER1	(RT305X_INTC_IRQ_BASE + 2)
#define RT305X_INTC_IRQ_IA	(RT305X_INTC_IRQ_BASE + 3)
#define RT305X_INTC_IRQ_PCM	(RT305X_INTC_IRQ_BASE + 4)
#define RT305X_INTC_IRQ_UART0	(RT305X_INTC_IRQ_BASE + 5)
#define RT305X_INTC_IRQ_PIO	(RT305X_INTC_IRQ_BASE + 6)
#define RT305X_INTC_IRQ_DMA	(RT305X_INTC_IRQ_BASE + 7)
#define RT305X_INTC_IRQ_NAND	(RT305X_INTC_IRQ_BASE + 8)
#define RT305X_INTC_IRQ_PERFC	(RT305X_INTC_IRQ_BASE + 9)
#define RT305X_INTC_IRQ_I2S	(RT305X_INTC_IRQ_BASE + 10)
#define RT305X_INTC_IRQ_UART1	(RT305X_INTC_IRQ_BASE + 12)
#define RT305X_INTC_IRQ_ESW	(RT305X_INTC_IRQ_BASE + 17)
#define RT305X_INTC_IRQ_OTG	(RT305X_INTC_IRQ_BASE + 18)

extern void __iomem *rt305x_sysc_base;
extern void __iomem *rt305x_memc_base;

static inline void rt305x_sysc_wr(u32 val, unsigned reg)
{
	__raw_writel(val, rt305x_sysc_base + reg);
}

static inline u32 rt305x_sysc_rr(unsigned reg)
{
	return __raw_readl(rt305x_sysc_base + reg);
}

static inline void rt305x_memc_wr(u32 val, unsigned reg)
{
	__raw_writel(val, rt305x_memc_base + reg);
}

static inline u32 rt305x_memc_rr(unsigned reg)
{
	return __raw_readl(rt305x_memc_base + reg);
}

#endif /* _RT305X_H_ */
