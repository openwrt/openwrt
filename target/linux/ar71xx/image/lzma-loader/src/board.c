/*
 * LZMA compressed kernel loader for Atheros AR7XXX/AR9XXX based boards
 *
 * Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <stddef.h>
#include "config.h"

#define READREG(r)	*(volatile unsigned int *)(r)
#define WRITEREG(r,v)	*(volatile unsigned int *)(r) = v

#define UART_BASE	0xb8020000

#define UART_TX		0
#define UART_LSR	5

#define UART_LSR_THRE   0x20

#define UART_READ(r)		READREG(UART_BASE + 4 * (r))
#define UART_WRITE(r,v)		WRITEREG(UART_BASE + 4 * (r), (v))

void board_putc(int ch)
{
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0);
	UART_WRITE(UART_TX, ch);
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0);
}

void board_init(void)
{
}
