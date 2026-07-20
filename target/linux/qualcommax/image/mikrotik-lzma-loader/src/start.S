/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * arm64 entry point of the LZMA loader, modelled on MikroTik's own loader as
 * found in the flash dump.
 *
 * Its entry sequence, taken from the disassembly:
 *     bl   .+4            ; get PC into x30
 *     mov  sp, x30        ; stack = own code address
 *     sub  sp, sp, #4
 *     mrs  x4, currentel
 *     cmp  x4, #4         ; EL1 (raw value, not shifted)
 *     mrs  x5, cpacr_el1
 *     orr  x5, x5, #0x300000
 *     msr  cpacr_el1, x5  ; enable FP/SIMD
 *     bl   main
 *
 * Two details matter here:
 *   1. CPACR_EL1 has to be unlocked; RouterBOOT hands over with FP/SIMD
 *      trapped.
 *   2. No DRAM address has to be guessed for the stack: it goes just below
 *      the load address, which RouterBOOT itself uses for the same purpose.
 *
 * RouterBOOT's MMU setup is torn down early because it does not map the load
 * address. main() then installs a small identity mapping of its own so that
 * decompression runs cached; jump_kernel() turns everything off again.
 */
	.section .text.start, "ax"
	.global _start
_start:
	msr	daifset, #0xf			/* mask all interrupts */

	/*
	 * The watchdog is deliberately not disabled here but in main(), where its
	 * original state can be saved and restored before jumping to the kernel.
	 * Only microseconds pass until main() runs.
	 */

	/*
	 * Stack just below our own code, as MikroTik's loader does.
	 *
	 * Do NOT move this into .bss to get it inside the cacheable mapping.
	 * That was tried and the loader stops booting - RouterBOOT loads only
	 * filesz bytes, and the memory beyond that is evidently not usable this
	 * early, before the .bss clearing loop has run. The area below the load
	 * address on the other hand is known-good: it is where RouterBOOT's own
	 * loader puts its stack.
	 *
	 * The cost is that the stack stays uncached, since everything below
	 * 0x01100000 has to remain Device-mapped for the TLMM registers.
	 */
	bl	1f
1:	mov	sp, x30
	sub	sp, sp, #16
	mov	x6, sp
	and	x6, x6, #0xfffffffffffffff0	/* force 16-byte alignment */
	mov	sp, x6

	/* Enable FP/SIMD, EL1 only, as in the original */
	mrs	x4, CurrentEL
	cmp	x4, #4
	b.ne	2f
	mrs	x5, cpacr_el1
	orr	x5, x5, #0x300000
	msr	cpacr_el1, x5
	isb
2:
	/*
	 * Turn off MMU and D-cache. RouterBOOT's mapping does not cover the load
	 * address, so the first access there faults while the MMU is on. Write
	 * back the caches first so RouterBOOT's pending stores are not lost.
	 */
	mov	x20, x30
	bl	dcache_flush_all
	mov	x30, x20

	mrs	x9, CurrentEL
	cmp	x9, #4				/* EL1 */
	b.ne	21f
	mrs	x10, sctlr_el1
	bic	x10, x10, #1			/* M */
	bic	x10, x10, #4			/* C */
	msr	sctlr_el1, x10
	b	22f
21:	mrs	x10, sctlr_el2
	bic	x10, x10, #1
	bic	x10, x10, #4
	msr	sctlr_el2, x10
22:	isb
	ic	iallu
	tlbi	vmalle1
	dsb	sy
	isb

	/* Clear .bss */
	adrp	x0, __bss_start
	add	x0, x0, :lo12:__bss_start
	adrp	x1, __bss_end
	add	x1, x1, :lo12:__bss_end
3:	cmp	x0, x1
	b.hs	4f
	str	xzr, [x0], #8
	b	3b

4:	bl	main
5:	wfe
	b	5b

/*
 * mmu_enable(x0 = physical address of the level 1 table)
 *
 * Identity mapping only, so no address translation actually changes; the point
 * is purely to get cacheable attributes for the LZMA working set. Without this
 * every data access runs as Device-nGnRnE, because with the MMU off ARM64
 * gives no other choice.
 *
 * Table walks are marked non-cacheable (TCR IRGN0/ORGN0 = 0) so the tables may
 * live anywhere; they are written before this runs and never touched again.
 *
 * Only implemented for EL1. At any other exception level the function returns
 * 0 and the loader simply keeps running uncached, exactly as before.
 */
	.global mmu_enable
mmu_enable:
	mrs	x9, CurrentEL
	cmp	x9, #4				/* EL1 */
	b.eq	71f
	mov	x0, #0
	ret
71:	msr	ttbr0_el1, x0

	/* MAIR: Attr0 = Device-nGnRnE (0x00), Attr1 = Normal WB RW-allocate */
	mov	x1, #0xff00
	msr	mair_el1, x1

	/*
	 * TCR: T0SZ = 32 (4 GB VA, 4 KB granule -> level 1 start, 1 GB blocks),
	 * walks non-cacheable and non-shareable, TTBR1 walks disabled (EPD1).
	 */
	mov	x1, #32				/* T0SZ */
	orr	x1, x1, #(1 << 23)		/* EPD1 */
	msr	tcr_el1, x1
	isb

	tlbi	vmalle1
	dsb	nsh
	isb

	mrs	x1, sctlr_el1
	orr	x1, x1, #(1 << 0)		/* M  - MMU */
	orr	x1, x1, #(1 << 2)		/* C  - data cache */
	orr	x1, x1, #(1 << 12)		/* I  - instruction cache */
	msr	sctlr_el1, x1
	isb
	mov	x0, #1
	ret

/*
 * read_cntpct() / read_cntfrq() - generic timer, runs regardless of the MMU.
 * Used to report how long decompression actually took.
 */
	.global read_cntpct
read_cntpct:
	isb
	mrs	x0, cntpct_el0
	ret

	.global read_cntfrq
read_cntfrq:
	mrs	x0, cntfrq_el0
	ret

/*
 * jump_kernel(x0 = DTB, x1 = kernel) - does not return.
 * Write back the data cache, turn off MMU and caches, invalidate I-cache and
 * TLB, then follow the arm64 boot protocol: x0 = DTB, x1..x3 = 0.
 */
	.global jump_kernel
jump_kernel:
	mov	x21, x0
	mov	x22, x1
	msr	daifset, #0xf

	mov	x20, x30
	bl	dcache_flush_all
	mov	x30, x20

	mrs	x9, CurrentEL
	cmp	x9, #4				/* EL1 */
	b.ne	61f
	mrs	x10, sctlr_el1
	bic	x10, x10, #1			/* M */
	bic	x10, x10, #4			/* C */
	msr	sctlr_el1, x10
	b	62f
61:	mrs	x10, sctlr_el2
	bic	x10, x10, #1
	bic	x10, x10, #4
	msr	sctlr_el2, x10
62:	isb
	ic	iallu
	tlbi	vmalle1
	dsb	sy
	isb

	mov	x0, x21
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
	br	x22

/*
 * dcache_flush_all - clean and invalidate by set/way, the standard sequence.
 * MikroTik's loader contains the same loop (dc cisw across all levels).
 */
dcache_flush_all:
	dmb	sy
	mrs	x0, clidr_el1
	lsr	x3, x0, #23
	and	x3, x3, #7
	cbz	x3, 59f
	mov	x10, #0
50:	add	x2, x10, x10, lsr #1
	lsr	x1, x0, x2
	and	x1, x1, #7
	cmp	x1, #2
	b.lt	58f
	msr	csselr_el1, x10
	isb
	mrs	x1, ccsidr_el1
	and	x2, x1, #7
	add	x2, x2, #4
	mov	x4, #0x3ff
	and	x4, x4, x1, lsr #3
	clz	w5, w4
	mov	x7, #0x7fff
	and	x7, x7, x1, lsr #13
51:	mov	x9, x4
52:	lsl	x6, x9, x5
	orr	x11, x10, x6
	lsl	x6, x7, x2
	orr	x11, x11, x6
	dc	cisw, x11
	subs	x9, x9, #1
	b.ge	52b
	subs	x7, x7, #1
	b.ge	51b
58:	add	x10, x10, #2
	cmp	x3, x10
	b.gt	50b
59:	mov	x10, #0
	msr	csselr_el1, x10
	dsb	sy
	isb
	ret
