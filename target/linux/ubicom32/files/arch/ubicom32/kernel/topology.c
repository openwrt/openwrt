/*
 * arch/ubicom32/kernel/topology.c
 *   Ubicom32 architecture sysfs topology information.
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

#include <linux/init.h>
#include <linux/smp.h>
#include <linux/cpu.h>
#include <linux/cache.h>

static struct cpu cpu_devices[NR_CPUS] __read_mostly;

static int __init topology_init(void)
{
	int num;

	for_each_present_cpu(num) {
		cpu_devices[num].hotpluggable = 0;
		register_cpu(&cpu_devices[num], num);
	}
	return 0;
}

subsys_initcall(topology_init);
