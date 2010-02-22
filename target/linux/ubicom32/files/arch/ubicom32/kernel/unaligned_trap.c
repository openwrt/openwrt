/*
 * arch/ubicom32/kernel/unaligned_trap.c
 *   Handle unaligned traps in both user or kernel space.
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

#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/cacheflush.h>
#include <asm/traps.h>

#define FALSE 0
#define TRUE 1

/* no possible trap */
#define UNUSED 0
/* possible source operand trap */
#define SRC 1
#define SRC_2 2
/* possible destination operand trap */
#define DEST 3
#define DEST_2 4
/* can be either source or destination or both */
#define TWO_OP 5
#define TWO_OP_2 6

/* TODO: What is the real value here, put something in to make it compile for
 * now */
#define MOVE_2	0x0d
#define LSL_2	0x11
#define LSR_2	0x13
#define MOVEI	0x19
#define CMPI	0x18

static int op_format[32] =
{
	TWO_OP,		/* 0x00 */
	UNUSED,
	SRC,
	UNUSED,
	TWO_OP,		/* 0x04 */
	TWO_OP,
	SRC,
	UNUSED,
	TWO_OP_2,	/* 0x08 */
	TWO_OP,
	TWO_OP_2,
	TWO_OP,
	TWO_OP_2,	/* 0x0C */
	TWO_OP,
	TWO_OP_2,
	TWO_OP,
	TWO_OP,		/* 0x10 */
	TWO_OP_2,
	TWO_OP,
	TWO_OP,
	UNUSED,		/* 0x14 */
	UNUSED,
	UNUSED,
	UNUSED,
	SRC_2,		/* 0x18 */
	DEST_2,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x1C */
	UNUSED,
	UNUSED,		/* unaligned CALLI will not be fixed. */
	UNUSED
};

static int op_0_format[32] =
{
	UNUSED,		/* 0x00 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x04 - ret don't fix - bad ret is always wrong */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x08 */
	UNUSED,
	TWO_OP,
	TWO_OP_2,
	TWO_OP,		/* 0x0c */
	TWO_OP_2,
	TWO_OP,
	UNUSED,		/* .1 can't trap */
	UNUSED,		/* 0x10 */
	UNUSED,
	SRC,
	UNUSED,
	UNUSED,		/* 0x14 */
	TWO_OP_2,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x18 */
	UNUSED,
	UNUSED,
	UNUSED,
	DEST,		/* 0x1c */
	DEST,
	DEST,
	DEST,		/* all lea have 32-bit destination */
};

static int op_2_format[32] =
{
	UNUSED,		/* 0x00 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x04 */
	UNUSED,
	SRC,
	UNUSED,
	UNUSED,		/* 0x08 crcgen is .1 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x0c */
	UNUSED,
	UNUSED,
	UNUSED,
	SRC,		/* 0x10 */
	SRC_2,
	SRC,
	SRC_2,
	SRC,		/* 0x14 */
	SRC_2,
	SRC,
	UNUSED,
	UNUSED,		/* 0x18 */
	UNUSED,
	SRC,
	UNUSED,
	SRC,		/* 0x1c */
	UNUSED,
	SRC_2,
	UNUSED,
};

static int op_6_format[32] =
{
	SRC_2,		/* 0x00 */
	SRC_2,
	SRC_2,
	SRC_2,
	SRC_2,		/* 0x04 */
	SRC_2,
	UNUSED,
	SRC_2,
	SRC,		/* 0x08 MULS.4 */
	SRC_2,
	SRC,
	UNUSED,
	UNUSED,		/* 0x0c */
	UNUSED,
	UNUSED,
	UNUSED,
	SRC,		/* 0x10 */
	SRC_2,
	SRC,
	SRC_2,
	UNUSED,		/* 0x14 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x18 */
	UNUSED,
	UNUSED,
	UNUSED,
	UNUSED,		/* 0x1c */
	UNUSED,
	UNUSED,
	UNUSED,
};

/*
 * unaligned_get_address()
 *	get an address using save_an and save_dn registers, and updates save_an
 *	with side effects
 */
unsigned char *unaligned_get_address(int thread, int specifier, int four_byte,
				     unsigned int save_an[],
				     unsigned int save_dn[], int *write_back_an)
{
	unsigned char *address;

	int areg = (specifier >> 5) & 7;
	if ((specifier >> 8) == 2) {
		int offset = specifier & 0xf;
		offset = ((offset << 28) >> 28);
		if (likely(four_byte)) {
			offset <<= 2;
		} else {
			offset <<= 1;
		}
		if (specifier & 0x10) {
			address = (unsigned char *)(save_an[areg] + offset);
		} else {
			address = (unsigned char *)save_an[areg];
		}
		save_an[areg] = save_an[areg] + offset;

		/*
		 * Let caller know An registers have been modified.
		 */
		*write_back_an = 1;
	} else if ((specifier >> 8) == 3) {
		int dreg = specifier & 0xf;
		if (likely(four_byte)) {
			address = (unsigned char *)(save_an[areg] +
						    (save_dn[dreg] << 2));
		} else {
			address = (unsigned char *)(save_an[areg] +
						    (save_dn[dreg] << 1));
		}
	} else {
		int offset = ((specifier >> 3) & 0x60) | (specifier & 0x1f);
		if (likely(four_byte)) {
			address = (unsigned char *)(save_an[areg] +
						    (offset << 2));
		} else {
			address = (unsigned char *)(save_an[areg] +
						    (offset << 1));
		}
	}

	return address;
}

static	int save_dn[16];
static	int save_an[8];
static	int save_acc[5];

/*
 * unaligned_emulate()
 *	emulate the instruction at thread's pc that has taken an unaligned data
 *	trap.
 *
 * source or destination or both might be unaligned
 * the instruction must have a memory source or destination or both
 * the emulated instruction is copied and executed in this thread
 *
 *	TODO: Protection is handled outside of this function
 *	TODO: handling simultaneous unaligned and memory protection traps
 *
 *	Get thread state
 *		the PC and instruction (and local copy, emulate_inst), and An
 *		and Dn registers
 *		All implicit soruce state (source3, CSR, accumulators)

 *	if the instruction has a memory source
 *		Use the instruction, An and Dn registers to form src_address
 *		get unaligned source data from src_address (usually sign
 *		extended)
 *			(2 bytes, with or without sign extension, or 4 bytes)
 *		modify emulate_inst to use d0 as source
 *	else
 *		get the soure operand from one of thread's registers
 *	if instruction has a memory destination
 *		Use the instruction, An and Dn registers to form dest_address
 *		modify emulate_inst to use d0 as destination
 *	if there was a memory source
 *		put the source data in thread's d0
 *	get the source-2 Dn operand and source 3 operand from thread
 *	execute modified inst
 *		(save it, flush caches, set up local values for implicit
 *		sources, execute, save explicit and implicit results)
 *	if inst has destination address
 *		copy result to dest_address, possibly unaligned, 1, 2, or 4
 *		bytes
 *	restore thread's implicit results (modified address registers, CSR,
 *	accumulators) add 4 to thread's pc
 */
void unaligned_emulate(unsigned int thread)
{
	unsigned int pc;
	unsigned int inst;
	unsigned int op;
	unsigned int subop;
	int format;
	unsigned int emulate_inst;
	int four_byte;
	int src_operand, dest_operand;
	int save_csr;
	int source3;
	unsigned int source1;
	unsigned int source_data;
	unsigned char *dest_address = NULL;
	int source2 = 0;
	unsigned int result;
	unsigned int write_back_an = 0;
	unsigned int chip_id_copy;

	extern unsigned int trap_emulate;
	extern unsigned int ubicom32_emulate_insn(int source1, int source2,
						  int source3, int *save_acc,
						  int *save_csr);

	/*
	 * get the chip_id
	 */
	asm volatile (
	"	move.4		%0, chip_id		\n\t" /* get chip_id. */
		: "=r"(chip_id_copy)
		:
	);

	/*
	 * get the pc
	 */
	asm volatile (
	"	move.4		CSR, %1		\n\t" /* set source thread in
						       * CSR */
	"	setcsr_flush	0		\n\t"
	"	move.4		%0, pc		\n\t"
	"	move.4		CSR, #0		\n\t" /* restore CSR */
	"	setcsr_flush	0		\n\t"
		: "=a"(pc)
		: "d" ((1 << 8) | (thread << 9))
		: "cc"
	);

	inst = *((unsigned int *)pc);
	op = inst >> 27;
	if (unlikely(op == 2 || op == 6)) {
		subop = (inst >> 21) & 0x1f;
	} else {
		subop = (inst >> 11) & 0x1f;
	}
	format = op_format[op];
	emulate_inst = inst;

	if (op == 0) {
		format = op_0_format[subop];
	} else if (op == 2) {
		format = op_2_format[subop];
	} else if (op == 6) {
		format = op_6_format[subop];
	}

	if (unlikely(format == UNUSED)) {
		/*
		 * We are not going to emulate this. Bump PC by 4 and move on.
		 */
		asm volatile (
		"	move.4		CSR, %0			\n\t"
		"	setcsr_flush	0			\n\t"
		"	move.4		pc, %1			\n\t"
		"	setcsr		#0			\n\t"
		"	setcsr_flush	0			\n\t"
			:
			: "d"((1 << 14) | (thread << 15)), "d"(pc + 4)
			: "cc"
		);
		return;
	}

	four_byte = (format == TWO_OP || format == DEST || format == SRC);

	/*
	 * source or destination memory operand needs emulation
	 */
	src_operand = (format == SRC ||
		       format == SRC_2 ||
		       format == TWO_OP ||
		       format == TWO_OP_2) &&
		((inst >> 8) & 7) > 1;

	dest_operand = (format == DEST ||
			format == DEST_2 ||
			format == TWO_OP ||
			format == TWO_OP_2) &&
		((inst >> 24) & 7) > 1;

	/*
	 * get thread's implicit sources (not covered by source context select).
	 * data and address registers and CSR (for flag bits) and src3 and
	 * accumulators
	 */
	asm volatile (
	"	move.4		CSR, %2		\n\t"	/* set source thread in
							 * CSR */
	"	setcsr_flush	0		\n\t"
	"	move.4		(%3), d0	\n\t"	/* get dn registers */
	"	move.4		4(%3), d1	\n\t"
	"	move.4		8(%3), d2	\n\t"
	"	move.4		12(%3), d3	\n\t"
	"	move.4		16(%3), d4	\n\t"
	"	move.4		20(%3), d5	\n\t"
	"	move.4		24(%3), d6	\n\t"
	"	move.4		28(%3), d7	\n\t"
	"	move.4		32(%3), d8	\n\t"
	"	move.4		36(%3), d9	\n\t"
	"	move.4		40(%3), d10	\n\t"
	"	move.4		44(%3), d11	\n\t"
	"	move.4		48(%3), d12	\n\t"
	"	move.4		52(%3), d13	\n\t"
	"	move.4		56(%3), d14	\n\t"
	"	move.4		60(%3), d15	\n\t"
	"	move.4		(%4), a0	\n\t"	/* get an registers */
	"	move.4		4(%4), a1	\n\t"
	"	move.4		8(%4), a2	\n\t"
	"	move.4		12(%4), a3	\n\t"
	"	move.4		16(%4), a4	\n\t"
	"	move.4		20(%4), a5	\n\t"
	"	move.4		24(%4), a6	\n\t"
	"	move.4		28(%4), a7	\n\t"
	"	move.4		%0, CSR		\n\t"	/* get csr and source3
							 * implicit operands */
	"	move.4		%1, source3	\n\t"
	"	move.4		(%5), acc0_lo	\n\t"	/* get accumulators */
	"	move.4		4(%5), acc0_hi	\n\t"
	"	move.4		8(%5), acc1_lo	\n\t"
	"	move.4		12(%5), acc1_hi	\n\t"
	"	move.4		16(%5), mac_rc16	\n\t"
	"	move.4		CSR, #0		\n\t"	/* restore CSR */
	"	setcsr_flush	0		\n\t"
		: "=m"(save_csr), "=m"(source3)
		: "d"((1 << 8) | (thread << 9)),
		  "a"(save_dn), "a"(save_an), "a"(save_acc)
		: "cc"
	);

	/*
	 * turn off thread select bits if they were on
	 */
	BUG_ON((save_csr & 0x04100) != 0);
	if (unlikely(save_csr & 0x04100)) {
		/*
		 * Things are in funny state as thread select bits are on in
		 * csr. PANIC.
		 */
		panic("In unaligned trap handler. Trap thread CSR has thread "
		      "select bits on.\n");
	}

	save_csr = save_csr & 0x1000ff;

	/*
	 * get the source1 operand
	 */
	source1 = 0;
	if (src_operand) {
		unsigned char *src_address;

		/*
		 * source1 comes from memory
		 */
		BUG_ON(!(format == TWO_OP || format == TWO_OP_2 ||
			 format == SRC || format == SRC_2));
		src_address = unaligned_get_address(thread, inst & 0x7ff,
						    four_byte, save_an,
						    save_dn, &write_back_an);

		/*
		 * get data (possibly unaligned)
		 */
		if (likely(four_byte)) {
			source_data = (*src_address << 24) |
				(*(src_address + 1) << 16) |
				(*(src_address + 2) << 8) |
				*(src_address + 3);
			source1 = source_data;
		} else {
			source1 = *src_address << 8 |
				*(src_address + 1);

			/*
			 * Source is not extended if the instrution is MOVE.2 or
			 * if the cpu CHIP_ID >= 0x30000 and the instruction is
			 * either LSL.2 or LSR.2.  All other cases have to be
			 * sign extended.
			 */
			if ((!(op == 2 && subop == MOVE_2)) &&
			    (!((chip_id_copy >= 0x30000) &&
			       (subop == LSL_2 || subop == LSR_2)))) {
				/*
				 * Have to sign extend the .2 entry.
				 */
				source1 = ((unsigned int)
					   ((signed int)
					    ((signed short) source1)));
			}
		}
	} else if (likely(op != MOVEI)) {
		/*
		 * source1 comes from a register, using move.4 d0, src1
		 * unaligned_emulate_get_source is pointer to code to insert remulated instruction
		 */
		extern unsigned int unaligned_emulate_get_src;
		*((int *)&unaligned_emulate_get_src) &= ~(0x7ff);
		*((int *)&unaligned_emulate_get_src) |= (inst & 0x7ff);
		flush_dcache_range((unsigned long)(&unaligned_emulate_get_src),
				   (unsigned long)(&unaligned_emulate_get_src) + 4);

		asm volatile (
			/* source1 uses thread's registers */
		"	move.4		CSR, %1			\n\t"
		"	setcsr_flush 0				\n\t"
		"unaligned_emulate_get_src:			\n\t"
		"	move.4	%0, #0				\n\t"
		"	setcsr		#0			\n\t"
		"	setcsr_flush	0			\n\t"
			: "=d" (source1)
			: "d" ((1 << 8) | (thread << 9))
			: "cc"
		);
	}

	/*
	 * get the destination address
	 */
	if (dest_operand) {
		BUG_ON(!(format == TWO_OP || format == TWO_OP_2 ||
			 format == DEST || format == DEST_2));
		dest_address = unaligned_get_address(thread,
						     ((inst >> 16) & 0x7ff),
						     four_byte, save_an,
						     save_dn, &write_back_an);
	}

	if (write_back_an) {
		/*
		 * restore any modified An registers
		 */
		asm volatile (
		"	move.4		CSR, %0			\n\t"
		"	setcsr_flush	0			\n\t"
		"	move.4		a0, (%1)		\n\t"
		"	move.4		a1, 4(%1)		\n\t"
		"	move.4		a2, 8(%1)		\n\t"
		"	move.4		a3, 12(%1)		\n\t"
		"	move.4		a4, 16(%1)		\n\t"
		"	move.4		a5, 20(%1)		\n\t"
		"	move.4		a6, 24(%1)		\n\t"
		"	move.4		a7, 28(%1)		\n\t"
		"	setcsr		#0			\n\t"
		"	setcsr_flush	0			\n\t"
			:
			: "d" ((1 << 14) | (thread << 15)), "a" (save_an)
			: "cc"
		);
	}

	/*
	 * get source 2 register if needed, and modify inst to use d1 for
	 * source-2 source-2 will come from this thread, not the trapping thread
	 */
	source2 = 0;
	if ((op >= 8 && op <= 0x17) ||
	    ((op == 2 || op == 6) && (inst & 0x4000000))) {
		int src_dn = (inst >> 11) & 0xf;
		source2 = save_dn[src_dn];
		/*
		 * force the emulated instruction to use d1 for source2 operand
		 */
		emulate_inst = (emulate_inst & 0xffff07ff) | 0x800;
	}

	if (likely(op != MOVEI)) {
		/*
		 * change emulated instruction source1 to d0
		 */
		emulate_inst &= ~0x7ff;
		emulate_inst |= 1 << 8;
	}

	if (unlikely(op == 6 || op == 2)) {
		/*
		 * Set destination to d0
		 */
		emulate_inst &= ~(0xf << 16);
	} else if (likely(op != CMPI)) {
		/*
		 * Set general destination field to d0.
		 */
		emulate_inst &= ~(0x7ff << 16);
		emulate_inst |= 1 << 24;
	}

	/*
	 * execute emulated instruction d0, to d0, no memory access
	 * source2 if needed will be in d1
	 * source3, CSR, and accumulators are set up before execution
	 */
	*((unsigned int *)&trap_emulate) = emulate_inst;
	flush_dcache_range((unsigned long)(&trap_emulate),
			   (unsigned long)(&trap_emulate) + 4);

	result = ubicom32_emulate_insn(source1, source2, source3,
				       save_acc, &save_csr);

	/*
	 * set the result value
	 */
	if (dest_operand) {
		/*
		 * copy result to memory
		 */
		if (four_byte) {
			*dest_address++ =
				(unsigned char)((result >> 24) & 0xff);
			*dest_address++ =
				(unsigned char)((result >> 16) & 0xff);
		}
		*dest_address++ = (unsigned char)((result >> 8) & 0xff);
		*dest_address = (unsigned char)(result & 0xff);
	} else if (likely(op != CMPI)) {
		/*
		 * copy result to a register, using move.4 dest, result
		 */
		extern unsigned int unaligned_trap_set_result;
		*((unsigned int *)&unaligned_trap_set_result) &= ~0x7ff0000;

		if (op == 2 || op == 6) {
			*((unsigned int *)&unaligned_trap_set_result) |=
				((inst & 0x000f0000) | 0x01000000);
		} else {
			*((unsigned int *)&unaligned_trap_set_result) |=
				(inst & 0x7ff0000);
		}
		flush_dcache_range((unsigned long)&unaligned_trap_set_result,
				   ((unsigned long)(&unaligned_trap_set_result) + 4));

		asm volatile (
			/* result uses thread's registers */
		"	move.4		CSR, %1			\n\t"
		"	setcsr_flush 0				\n\t"
		"unaligned_trap_set_result:			\n\t"
		"	move.4 #0, %0				\n\t"
		"	setcsr		#0			\n\t"
		"	setcsr_flush	0			\n\t"
			:
			: "d"(result), "d" ((1 << 14) | (thread << 15))
			: "cc"
		);
	}

	/*
	 * bump PC in thread and restore implicit register changes
	 */
	asm volatile (
	"	move.4		CSR, %0			\n\t"
	"	setcsr_flush	0			\n\t"
	"	move.4		pc, %1			\n\t"
	"	move.4		acc0_lo, (%3)		\n\t"
	"	move.4		acc0_hi, 4(%3)		\n\t"
	"	move.4		acc1_lo, 8(%3)		\n\t"
	"	move.4		acc1_hi, 12(%3)		\n\t"
	"	move.4		mac_rc16, 16(%3)	\n\t"
	"	move.4		CSR, %2			\n\t"
	"	setcsr		#0			\n\t"
	"	setcsr_flush	0			\n\t"
		:
		: "d"((1 << 14) | (thread << 15)),
		  "d"(pc + 4), "d"(save_csr), "a"(save_acc)
		: "cc"
	);
}

/*
 * unaligned_only()
 *	Return true if either of the unaligned causes are set (and no others).
 */
int unaligned_only(unsigned int cause)
{
	unsigned int unaligned_cause_mask =
		(1 << TRAP_CAUSE_DST_MISALIGNED) |
		(1 << TRAP_CAUSE_SRC1_MISALIGNED);

	BUG_ON(cause == 0);
	return (cause & unaligned_cause_mask) == cause;
}
