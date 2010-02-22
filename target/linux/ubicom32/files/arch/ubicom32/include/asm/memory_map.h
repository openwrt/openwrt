/*
 * arch/ubicom32/include/asm/memory_map.h
 *   Machine memory maps/
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
#ifndef _ASM_UBICOM32_MEMORY_MAP_H
#define _ASM_UBICOM32_MEMORY_MAP_H

/*
 * Memory Size
 */
#define OCM_SECTOR_SIZE	0x00008000		/* 32K */

#if defined(CONFIG_UBICOM32_V3)
#define OCMSIZE	0x00030000	/* 192K on-chip RAM for both program and data */
#elif defined(CONFIG_UBICOM32_V4)
#define OCMSIZE	0x0003C000	/* 240K on-chip RAM for both program and data */
#else
#error "Unknown IP5K silicon"
#endif

#define OCMSTART	0x3ffc0000 /* alias from 0x03000000 for easy
				    * jump to/from SDRAM */
#define OCMEND		(OCMSTART + OCMSIZE)

#define SDRAMSTART	0x40000000

#define KERNELSTART	(SDRAMSTART + 0x00400000)

#define FLASHSTART	0x60000000

/*
 * CODELOADER / OS_SYSCALL OCM Reservations
 * Don't change these unless you know what you are doing.
 */
#define CODELOADER_SIZE  0x30
#define CODELOADER_BEGIN OCMSTART /* Must be OCM start for gdb to work. */
#define CODELOADER_END	 (CODELOADER_BEGIN + CODELOADER_SIZE)

#define OS_SYSCALL_BEGIN CODELOADER_END	/* system_call at this address */
#define OS_SYSCALL_SIZE  (512 - CODELOADER_SIZE)
#define OS_SYSCALL_END	 (OS_SYSCALL_BEGIN + OS_SYSCALL_SIZE)

#endif /* _ASM_UBICOM32_MEMORY_MAP_H */
