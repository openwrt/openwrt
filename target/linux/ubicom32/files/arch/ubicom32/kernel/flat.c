/*
 * arch/ubicom32/kernel/flat.c
 *   Ubicom32 architecture flat executable format support.
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
#include <linux/types.h>
#include <linux/flat.h>

unsigned long ubicom32_flat_get_addr_from_rp(unsigned long *rp,
					     u32_t relval,
					     u32_t flags,
					     unsigned long *persistent)
{
	u32_t relval_reloc_type = relval >> 27;
	u32_t insn = *rp;

	if (*persistent) {
		/*
		 * relval holds the relocation that has to be adjusted.
		 */
		if (relval == 0) {
			*persistent = 0;
		}

		return relval;
	}

	if (relval_reloc_type == R_UBICOM32_32) {
		/*
		 * insn holds the relocation
		 */
		return insn;
	}

	/*
	 * We don't know this one.
	 */
	return 0;
}

void ubicom32_flat_put_addr_at_rp(unsigned long *rp,
				  u32_t val,
				  u32_t relval,
				  unsigned long *persistent)
{
	u32_t reloc_type = (relval >> 27) & 0x1f;
	u32_t insn = *rp;

	/*
	 * If persistent is set then it contains the relocation type.
	 */
	if (*persistent) {
		/*
		 * If persistent is set then it contains the relocation type.
		 */
		reloc_type = (*persistent >> 27) & 0x1f;
	}

	switch (reloc_type) {
	case R_UBICOM32_32:
		/*
		 * Store the 32 bits as is.
		 */
		*rp = val;
		break;
	case R_UBICOM32_HI24:
		{
			/*
			 * 24 bit relocation that is part of the MOVEAI
			 * instruction. The 24 bits come from bits 7 - 30 of the
			 * relocation. The 24 bits eventually get split into 2
			 * fields in the instruction encoding.
			 *
			 * - Bits 7 - 27 of the relocation are encoded into bits
			 * 0 - 20 of the instruction.
			 *
			 * - Bits 28 - 30 of the relocation are encoded into bit
			 * 24 - 26 of the instruction.
			 */
			u32_t mask = 0x1fffff | (0x7 << 24);
			u32_t valid24bits = (val >> 7) & 0xffffff;
			u32_t bot_21 = valid24bits & 0x1fffff;
			u32_t upper_3_bits = ((valid24bits & 0xe00000) << 3);
			insn &= ~mask;

			insn |= bot_21;
			insn |= upper_3_bits;
			*rp = insn;
		}
		break;
	case R_UBICOM32_LO7_S:
	case R_UBICOM32_LO7_2_S:
	case R_UBICOM32_LO7_4_S:
		{
			/*
			 * Bits 0 - 6 of the relocation are encoded into the
			 * 7bit unsigned immediate fields of the SOURCE-1 field
			 * of the instruction.  The immediate value is left
			 * shifted by (0, 1, 2) based on the operand size.
			 */
			u32_t mask = 0x1f | (0x3 << 8);
			u32_t bottom, top;
			val &= 0x7f;
			if (reloc_type == R_UBICOM32_LO7_2_S) {
				val >>= 1;
			} else if (reloc_type == R_UBICOM32_LO7_4_S) {
				val >>= 2;
			}

			bottom  = val & 0x1f;
			top = val >> 5;
			insn &= ~mask;
			insn |= bottom;
			insn |= (top << 8);
			BUG_ON(*rp != insn);
			*rp = insn;
			break;
		}
	case R_UBICOM32_LO7_D:
	case R_UBICOM32_LO7_2_D:
	case R_UBICOM32_LO7_4_D:
		{
			/*
			 * Bits 0 - 6 of the relocation are encoded into the
			 * 7bit unsigned immediate fields of the DESTINATION
			 * field of the instruction.  The immediate value is
			 * left shifted by (0, 1, 2) based on the operand size.
			 */
			u32_t mask = (0x1f | (0x3 << 8)) << 16;
			u32_t bottom, top;
			val &= 0x7f;
			if (reloc_type == R_UBICOM32_LO7_2_D) {
				val >>= 1;
			} else if (reloc_type == R_UBICOM32_LO7_4_D) {
				val >>= 2;
			}
			bottom  = (val & 0x1f) << 16;
			top = (val >> 5) << 16;
			insn &= ~mask;
			insn |= bottom;
			insn |= (top << 8);
			BUG_ON(*rp != insn);
			*rp = insn;
			break;
		}
	case R_UBICOM32_LO7_CALLI:
	case R_UBICOM32_LO16_CALLI:
		{
			/*
			 * Extract the offset for a CALLI instruction. The
			 * offsets can be either 7 bits or 18 bits. Since all
			 * instructions in ubicom32 architecture are at work
			 * aligned addresses the truncated offset is right
			 * shifted by 2 before being encoded in the instruction.
			 */
			if (reloc_type == R_UBICOM32_LO7_CALLI) {
				val &= 0x7f;
			} else {
				val &= 0x3ffff;
			}

			val >>= 2;

			insn &= ~0x071f071f;
			insn |= (val & 0x1f) << 0;
			val >>= 5;
			insn |= (val & 0x07) << 8;
			val >>= 3;
			insn |= (val & 0x1f) << 16;
			val >>= 5;
			insn |= (val & 0x07) << 24;
			if (reloc_type == R_UBICOM32_LO7_CALLI) {
				BUG_ON(*rp != insn);
			}
			*rp = insn;
		}
		break;
	}

	if (*persistent) {
		*persistent = 0;
	}
}
