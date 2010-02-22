/*
 * arch/ubicom32/include/asm/cpu.h
 *   CPU definitions for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 2004-2005 ARM Ltd.
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
#ifndef _ASM_UBICOM32_CPU_H
#define _ASM_UBICOM32_CPU_H

#include <linux/percpu.h>

struct cpuinfo_ubicom32 {
	unsigned long tid;			/* Hardware thread number */

#ifdef CONFIG_SMP
	volatile unsigned long ipi_pending;	/* Bit map of operations to execute */
	unsigned long ipi_count;		/* Number of IPI(s) taken on this cpu */
#endif
};

DECLARE_PER_CPU(struct cpuinfo_ubicom32, cpu_data);

#endif /* _ASM_UBICOM32_CPU_H */
