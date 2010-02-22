/*
 * arch/ubicom32/include/asm/ip5000-asm.h
 *	Instruction macros for the IP5000.
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

#ifndef _ASM_UBICOM32_IP5000_ASM_H
#define _ASM_UBICOM32_IP5000_ASM_H

#if !defined(__LINKER__)

#if defined(__ASSEMBLY__)
.macro	cycles	quant
.if	(\quant) == 1
	nop
.else
.if	(((\quant) + 3) / 8) > 0
.rept	(((\quant) + 3) / 8)
	jmpt.f		.+4
.endr
.endif
.if	((((\quant) + 3) % 8) / 4) > 0
	jmpt.t		.+4
.endif
.endif
.endm
#else
/*
 * Same macro as above just in C inline asm
 */
asm ("					\n\
.macro	cycles	quant			\n\
.if	(\\quant) == 1			\n\
	nop				\n\
.else					\n\
.if	(((\\quant) + 3) / 8) > 0	\n\
.rept	(((\\quant) + 3) / 8)		\n\
	jmpt.f		.+4		\n\
.endr					\n\
.endif					\n\
.if	((((\\quant) + 3) % 8) / 4) > 0	\n\
	jmpt.t		.+4		\n\
.endif					\n\
.endif					\n\
.endm					\n\
");
#endif


#if defined(__ASSEMBLY__)
.macro	pipe_flush	cyc
	cycles		11 - (\cyc)
.endm
#else
/*
 * Same macro as above just in C inline asm
 */
asm ("					\n\
.macro	pipe_flush	cyc		\n\
	cycles		11 - (\\cyc)	\n\
.endm					\n\
");

#endif

#if defined(__ASSEMBLY__)
.macro	setcsr_flush	cyc
	cycles		5 - (\cyc)
.endm
#else
/*
 * Same macro as above just in C inline asm
 */
asm ("					\n\
.macro	setcsr_flush	cyc		\n\
	cycles		5 - (\\cyc)	\n\
.endm					\n\
");
#endif

/*
 * Macros for prefetch (using miss-aligned memory write)
 */
#if defined(__ASSEMBLY__)

.macro	pre_fetch_macro	thread_num, Ascratch, Aaddress length
	bclr		MT_TRAP_EN, MT_TRAP_EN, #(\thread_num)
	bset		\Ascratch, \Aaddress, #0	; force a miss-aligned address
	jmpt.t		.+4				; delay for both address setup and trap disable
	move.4		(\Ascratch), #0
	.if		(\length > 32)
	move.4		32(\Ascratch), #0
	.endif
	.if		(\length > 64)
	move.4		64(\Ascratch), #0
	.endif
	.if		(\length > 96)
	move.4		96(\Ascratch), #0
	.endif
	.if		(\length > 128)
	invalid_instruction				; maximum pre-fetch size is 4 cache lines
	.endif
	bset		MT_TRAP_EN, MT_TRAP_EN, #(\thread_num)
.endm

#else
/*
 * Same macro as above just in C inline asm
 */
asm ("								\n\
.macro	pre_fetch_macro	thread_num, Ascratch, Aaddress length	\n\
	bclr		MT_TRAP_EN, MT_TRAP_EN, #(\thread_num)	\n\
	bset		\\Ascratch, \\Aaddress, #0	; force a miss-aligned address \n\
	jmpt.t		.+4				; delay for both address setup and trap disable \n\
	move.4		(\\Ascratch), #0			\n\
	.if		(\\length > 32)				\n\
	move.4		32(\\Ascratch), #0			\n\
	.endif							\n\
	.if		(\\length > 64)				\n\
	move.4		64(\\Ascratch), #0			\n\
	.endif							\n\
	.if		(\\length > 96)				\n\
	move.4		96(\\Ascratch), #0			\n\
	.endif							\n\
	.if		(\\length > 128)			\n\
	invalid_instruction				; maximum pre-fetch size is 4 cache lines \n\
	.endif							\n\
	bset		MT_TRAP_EN, MT_TRAP_EN, #(\\thread_num)	\n\
.endm								\n\
");
#endif

#endif /* !defined(__LINKER__) */
#endif /* defined _ASM_UBICOM32_IP5000_ASM_H */
