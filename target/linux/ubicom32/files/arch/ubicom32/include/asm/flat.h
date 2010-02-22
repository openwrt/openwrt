/*
 * arch/ubicom32/include/asm/flat.h
 *   Definitions to support flat-format executables.
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

#ifndef _ASM_UBICOM32_FLAT_H
#define _ASM_UBICOM32_FLAT_H

#define ARCH_FLAT_ALIGN 0x80
#define ARCH_FLAT_ALIGN_TEXT 1

#define  R_UBICOM32_32		2
#define  R_UBICOM32_HI24	7
#define  R_UBICOM32_LO7_S	8
#define  R_UBICOM32_LO7_2_S	9
#define  R_UBICOM32_LO7_4_S	10
#define  R_UBICOM32_LO7_D	11
#define  R_UBICOM32_LO7_2_D	12
#define  R_UBICOM32_LO7_4_D	13
#define  R_UBICOM32_LO7_CALLI	15
#define  R_UBICOM32_LO16_CALLI	16

extern void ubicom32_flat_put_addr_at_rp(unsigned long *rp, u32_t val, u32_t rval, unsigned long  *p);
extern unsigned long ubicom32_flat_get_addr_from_rp(unsigned long *rp, u32_t relval, u32_t flags, unsigned long *p);

#define	flat_stack_align(sp)			/* nothing needed */
#define	flat_argvp_envp_on_stack()		1
#define	flat_old_ram_flag(flags)		(flags)
#define	flat_reloc_valid(reloc, size)		((reloc) <= (size))
#define	flat_get_addr_from_rp(rp, relval, flags, p)	(ubicom32_flat_get_addr_from_rp(rp, relval,flags, p))
#define	flat_put_addr_at_rp(rp, val, relval)	do {ubicom32_flat_put_addr_at_rp(rp, val, relval, &persistent);} while(0)
#define	flat_get_relocate_addr(rel)		((persistent) ? (persistent & 0x07ffffff) : (rel & 0x07ffffff))

static inline int flat_set_persistent(unsigned int relval, unsigned long *p)
{
	if (*p) {
		return 0;
	} else {
		if ((relval >> 27) != R_UBICOM32_32) {
			/*
			 * Something other than UBICOM32_32. The next entry has the relocation.
			 */
			*p = relval;
			return 1;
		}
	}
	return 0;
}

#endif /* _ASM_UBICOM32_FLAT_H */
