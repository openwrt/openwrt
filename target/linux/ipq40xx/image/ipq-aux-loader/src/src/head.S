.globl _start
_start:
	mov r7, pc @save REAL _start addr
	sub	r7, r7, #8

	mov r6, r1 @save ARGS[1] to r6(ARCH value)

	@mov r0, r7
	@bl owl_debug

	ldr r0, =(CONFIG_SYS_TEXT_BASE)
	mov r1, r7 @ real _start addr !
	mov r2, r6 @ARCH value
	@bl owl_debug
	bl relocate_code

.globl _payload_data_begin_ofs
_payload_data_begin_ofs:
	.word _payload_data_begin - _start

.globl _payload_data_end_ofs
_payload_data_end_ofs:
	.word _payload_data_end - _start

/*
 * void relocate_code (dst, REAL _start addr, ARCH)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 */
.globl relocate_code
.align		4
relocate_code:
	mov	r6, r0	/* save addr of destination */
	mov	r5, r1	/* REAL _start addr */
	mov	r4, r2	/* ARCH */

	/* Set up the stack						    */
stack_setup:
	mov	sp, r6

	bl watchdog_resetup /* resetup watchdog timer */

	ldr	r0, _payload_data_begin_ofs
	add	r0, r0, r5 /* add in memory offset */
	mov	r1, r6			/* r1 <- scratch for copy_loop */
	ldr	r2, _payload_data_end_ofs
	add	r2, r2, r5 /* add in memory offset */

	@bl owl_debug
copy_loop:
	ldmia	r0!, {r9-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r9-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end address [r2]    */
	blo	copy_loop

/* Bss reset will be made by the target called function(start.S) */

/*
 * We are done. Do not return, instead branch to second part of board
 * initialization, now running from RAM.
 */
jump_2_ram:
/*
 * If I-cache is enabled invalidate it.
 * In our case(RouterBOOT) it should be turned off
 * anyway, but let it be.
 */
#ifndef CONFIG_SYS_ICACHE_OFF
	mov r0, #0
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr     p15, 0, r0, c7, c10, 4	@ DSB
	mcr     p15, 0, r0, c7, c5, 4	@ ISB
#endif
	ldr	r0, =(CONFIG_SYS_TEXT_BASE)
	mov r1, r5 /* REAL _start addr */
	mov r2, r4 /* ARCH */
	/* jump to it ... */
	mov	pc, r0

.type relocate_code STT_FUNC
.size relocate_code, .-relocate_code
