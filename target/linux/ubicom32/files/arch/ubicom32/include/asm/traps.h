/*
 * arch/ubicom32/include/asm/traps.h
 *   Trap related definitions for Ubicom32 architecture.
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

#ifndef _ASM_UBICOM32_TRAPS_H
#define _ASM_UBICOM32_TRAPS_H

/*
 * Trap causes passed from ultra to Host OS
 */
#define TRAP_CAUSE_TOTAL		13
#define TRAP_CAUSE_DST_RANGE_ERR	12
#define TRAP_CAUSE_SRC1_RANGE_ERR	11
#define TRAP_CAUSE_I_RANGE_ERR		10
#define TRAP_CAUSE_DCAPT		9
#define TRAP_CAUSE_DST_SERROR		8
#define TRAP_CAUSE_SRC1_SERROR		7
#define TRAP_CAUSE_DST_MISALIGNED	6
#define TRAP_CAUSE_SRC1_MISALIGNED	5
#define TRAP_CAUSE_DST_DECODE_ERR	4
#define TRAP_CAUSE_SRC1_DECODE_ERR	3
#define TRAP_CAUSE_ILLEGAL_INST		2
#define TRAP_CAUSE_I_SERROR		1
#define TRAP_CAUSE_I_DECODE_ERR		0

extern void trap_handler(int irq, struct pt_regs *regs);
extern void trap_init_interrupt(void);
extern void unaligned_emulate(unsigned int thread);
extern int unaligned_only(unsigned int cause);

#endif /* _ASM_UBICOM32_TRAPS_H */
