/*
 * arch/ubicom32/lib/delay.c
 *   Ubicom32 implementation of udelay()
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

#include <linux/module.h>
#include <asm/param.h>
#include <asm/delay.h>
#include <asm/ip5000.h>

/*
 * read_current_timer()
 *	Return the current value of sysval.
 */
int __devinit read_current_timer(unsigned long *timer_val)
{
	*timer_val = (long)(UBICOM32_IO_TIMER->sysval);
	return 0;
}


void udelay(unsigned long usecs)
{
	_udelay(usecs);
}
EXPORT_SYMBOL(udelay);
