/*
 *  Realtek RLX based SoC early printk support
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/io.h>
#include <linux/errno.h>
#include <linux/serial_reg.h>
#include <asm/addrspace.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>

static void (*_prom_putchar)(unsigned char);

static void __iomem *realtek_uart0_base;
static int uart0_reg_shift_bits;

static inline void prom_putchar_wait(void __iomem *reg, u32 mask, u32 val)
{
	u32 t;

	do {
		t = __raw_readl(reg) >> uart0_reg_shift_bits;
		if ((t & mask) == val)
			break;
	} while (1);
}

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)

static void prom_putchar_real(unsigned char ch)
{
	prom_putchar_wait(realtek_uart0_base + UART_LSR * 4, BOTH_EMPTY, BOTH_EMPTY);
	__raw_writel(ch << uart0_reg_shift_bits, realtek_uart0_base + UART_TX * 4);
	prom_putchar_wait(realtek_uart0_base + UART_LSR * 4, BOTH_EMPTY, BOTH_EMPTY);
}

static void prom_putchar_init(void)
{
	realtek_uart0_base = ioremap_nocache(REALTEK_UART0_BASE,
					     REALTEK_UART0_SIZE);
	uart0_reg_shift_bits = 24;
	_prom_putchar = prom_putchar_real;
}

void prom_putchar(unsigned char ch)
{
	if (!_prom_putchar)
		prom_putchar_init();

	_prom_putchar(ch);
}
