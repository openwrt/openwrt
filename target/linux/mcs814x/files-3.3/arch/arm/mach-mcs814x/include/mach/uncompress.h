/*
 * Copyright (C) 2012, Florian Fainelli <florian@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_ARCH_UNCOMPRESS_H
#define __ASM_ARCH_UNCOMPRESS_H

#include <linux/serial_reg.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include <mach/cpu.h>

#define UART_SHIFT	(2)

/* cannot be static because the code will be inlined */
void __iomem *uart_base;

static inline void putc(int c)
{
	while (!(__raw_readb(uart_base + (UART_LSR << UART_SHIFT)) & UART_LSR_TEMT));
	__raw_writeb(c, uart_base + (UART_TX << UART_SHIFT));
}

static inline void flush(void)
{
}

static inline void arch_decomp_setup(void)
{
	if (soc_is_mcs8140())
		uart_base = (void __iomem *)(_PHYS_CONFADDR + _CONFOFFSET_UART);
}

#define arch_decomp_wdog()

#endif
