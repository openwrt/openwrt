/*
 * LZMA compressed kernel loader for Realtek 819X
 *
 * Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <stddef.h>


#define UART_THR        0xb8002000
#define UART_LSR        0xb8002014

#define REG32(reg)   (*(volatile unsigned int   *)((unsigned int)reg))


void serial_outc(char c)
{
        int i=0;

        while (1)
        {
                i++;
                if (i >=0x6000)
                        break;

                if (REG32(UART_LSR) & 0x20000000)
                        break;
        }
        REG32(UART_THR) = (c << 24);
}


void board_putc(int ch)
{
	serial_outc(ch);
}

void board_init(void)
{
}
