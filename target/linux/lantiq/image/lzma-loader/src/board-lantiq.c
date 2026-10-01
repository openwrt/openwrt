/*
 * Arch specific code for Lantiq based boards
 *
 * Copyright (C) 2013 John Crispin <blogic@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <stddef.h>
#include "config.h"

#define READREG(r)		*(volatile unsigned int *)(r)
#define WRITEREG(r,v)		*(volatile unsigned int *)(r) = v

#define UART_BASE		0xbe100c00
#define ASC_TBUF		(UART_BASE | 0x20)
#define ASC_FSTAT		(UART_BASE | 0x48)

#define TXMASK          0x3F00
#define TXOFFSET        8

#ifdef CONFIG_BOARD_BINTEC_BOSS
/*
 * The Bintec BOSS bootmonitor arms the hardware watchdog before handing
 * control to the image. The kernel does not pick it up early enough, so the
 * board would reset while still decompressing. Turn it off here.
 */
#define LTQ_WDT_MEMBASE_CR	0xbf8803f0	/* membase + control register */
#define LTQ_WDT_PW1		0x00BE0000
#define LTQ_WDT_PW2		0x00DC0000
#endif

void board_putc(char c)
{
	while ((READREG(ASC_FSTAT) & TXMASK) >> TXOFFSET);

	WRITEREG(ASC_TBUF, c);
}

void board_init(void)
{
#ifdef CONFIG_BOARD_BINTEC_BOSS
	/* Disable the watchdog armed by the BOSS bootmonitor */
	WRITEREG(LTQ_WDT_MEMBASE_CR, LTQ_WDT_PW1);	/* first magic word */
	WRITEREG(LTQ_WDT_MEMBASE_CR, LTQ_WDT_PW2);	/* power off WDT */
#endif
}
