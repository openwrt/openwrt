/*
 * arch/ubicom32/include/asm/ubicom32lcd.h
 *   Ubicom32 architecture LCD driver platform data definitions.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */
#ifndef _ASM_UBICOM32_UBICOM32_LCD_H
#define _ASM_UBICOM32_UBICOM32_LCD_H

#include <asm/ip5000.h>

struct ubicom32lcd_platform_data {
	int			pin_cs;
	int			pin_rs;
	int			pin_rd;
	int			pin_wr;
	int			pin_reset;
	int			data_shift;
	struct ubicom32_io_port	*port_data;
};

#endif /* _ASM_UBICOM32_UBICOM32_LCD_H */
