/*
 * arch/ubicom32/mach-ip7k/board-ip7500module.c
 *   Support for IP7500 CPU module board.
 *
 * This file supports the IP7500 CPU module board:
 *	8007-0510  Rev 1.0
 *	8007-0510A Rev 1.0 (with ethernet)
 *
 * DIP Switch SW2 configuration: (*) default
 *	POS 1: on(*) = PCI enabled, off = PCI disabled
 *	POS 2: on(*) = TTYX => PA6, off = TTYX => PF12
 *	POS 3: on(*) = TTYY => PA7, off = TTYY => PF15
 *	POS 4: unused
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/device.h>
#include <linux/gpio.h>
#include <asm/board.h>

/*
 * ip7500module_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7500module_init(void)
{
	board_init();

	ubi_gpio_init();

	return 0;
}

arch_initcall(ip7500module_init);
