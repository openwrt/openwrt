/*
 * Arch specific code for ramips based boards
 *
 * Copyright (C) 2013 John Crispin <blogic@openwrt.org>
 * Copyright (C) 2018 Tobias Schramm <tobleminer@gmail.com>
 * Copyright (C) 2023 Antonio Vázquez <antoniovazquezblanco@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <stdint.h>

#if defined(SOC_MT7620) || defined(SOC_RT3883)
#define UART_BASE			0xb0000c00
#define UART_THR			(UART_BASE + 0x04)
#define UART_LSR			(UART_BASE + 0x1c)
#define UART_LSR_THRE_MASK	0x40
#elif defined(SOC_MT7621)
#define UART_BASE			0xbe000c00
#define UART_THR			(UART_BASE + 0x00)
#define UART_LSR			(UART_BASE + 0x14)
#define UART_LSR_THRE_MASK	0x20
#elif defined(SOC_RT305X)
#define UART_BASE			0x10000500
#define UART_THR			(UART_BASE + 0x04)
#define UART_LSR			(UART_BASE + 0x1c)
#define UART_LSR_THRE_MASK	0x20
#else
#error "Unsupported SOC..."
#endif

// Helper functions
#define READREG(r)		(*(volatile uint32_t *)(r))
#define WRITEREG(r,v)	(*(volatile uint32_t *)(r)) = v


void board_init(void)
{
}

void board_putc(int ch)
{
	while ((READREG(UART_LSR) & UART_LSR_THRE_MASK) == 0);
	WRITEREG(UART_THR, ch);
	while ((READREG(UART_LSR) & UART_LSR_THRE_MASK) == 0);
}
