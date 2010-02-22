/*
 * arch/ubicom32/include/asm/machdep.h
 *   Machine dependent utility routines.
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
#ifndef _ASM_UBICOM32_MACHDEP_H
#define _ASM_UBICOM32_MACHDEP_H

#include <linux/interrupt.h>

/* Hardware clock functions */
extern unsigned long hw_timer_offset(void);

/* machine dependent power off functions */
extern void (*mach_reset)(void);
extern void (*mach_halt)(void);
extern void (*mach_power_off)(void);

extern void config_BSP(char *command, int len);

#endif /* _ASM_UBICOM32_MACHDEP_H */
