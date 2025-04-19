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
#include <stddef.h>

#define KSEG0			0x80000000
#define KSEG1			0xa0000000

#define _ATYPE_ 		__PTRDIFF_TYPE__
#define _ATYPE32_		int

#define _ACAST32_		(_ATYPE_)(_ATYPE32_)

#define CPHYSADDR(a)		((_ACAST32_(a)) & 0x1fffffff)

#define KSEG0ADDR(a)		(CPHYSADDR(a) | KSEG0)
#define KSEG1ADDR(a)		(CPHYSADDR(a) | KSEG1)

#define UART_BASE_ADDR		(0xb8002000)

void board_init(void)
{
}

void board_putc(int ch)
{
	while ((*((volatile unsigned int *)(UART_BASE_ADDR+0x14)) & 0x20000000) == 0);
	*((volatile unsigned char *)UART_BASE_ADDR) = ch;
}
