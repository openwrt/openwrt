/*
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */
#ifndef __ASM_MACH_REALTEK_IRQ_H
#define __ASM_MACH_REALTEK_IRQ_H

#define MIPS_CPU_IRQ_BASE		0
#define NR_IRQS				40

#define REALTEK_CPU_IRQ(_x)		(MIPS_CPU_IRQ_BASE + (_x))

#define REALTEK_SOC_IRQ_BASE		8
#define REALTEK_SOC_IRQ_COUNT		32
#define REALTEK_SOC_IRQ(_x)		(REALTEK_SOC_IRQ_BASE + (_x))


#define REALTEK_SOC_IRQ_CLOCK_TIMER	REALTEK_CPU_IRQ(7)

#include_next <irq.h>

#endif /* __ASM_MACH_REALTEK_IRQ_H */
