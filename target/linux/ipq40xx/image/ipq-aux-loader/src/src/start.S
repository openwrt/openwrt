.globl _start
_start:
	mov r7, r1 @save REAL _start addr
	mov r6, r2 @ARCH value

/* set the cpu to SVC32 mode */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr,r0

/* Move vector table
	 Set vector address in CP15 VBAR register */
	ldr     r0, =vector_table_stub
	mcr     p15, 0, r0, c12, c0, 0  @Set VBAR

/* Lets clear a bss */
clear_bss:
	ldr	r0, =_bss_start
	ldr	r1, =_bss_end
	mov	r2, #0x00000000		/* clear			    */
clbss_l:
	cmp	r0, r1			/* clear loop... */
	bhs	clbss_e			/* if reached end of bss, exit */
	str	r2, [r0]
	add	r0, r0, #4
	b	clbss_l
clbss_e:

/* Set stackpointer in internal RAM to call loader_main */
call_loader_main:
	/* stack grows left! <--SP--TEXT_BASE,_start--END of program */
	ldr	sp, =(CONFIG_SYS_TEXT_BASE)
	bic	sp, sp, #7 /* 8-byte alignment for ABI compliance */
	mov r0, r7 @REAL _start addr
	mov r1, r6 @ARCH
	bl	loader_main

.globl owl_get_sp
owl_get_sp:
	mov	r0, sp
	mov	pc, lr @ back to my caller

/* stub for IRQ vector table. we do not need
	 interrupt handlers, exception handlers, etc...
	 so if one of them happens, then just
	 wait(in deadloop) for the watchdogs reboot! */
.globl vector_table_stub
.align		4
vector_table_stub:
	b vector_table_stub @dead loop
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq
_undefined_instruction: .word vector_table_stub
_software_interrupt:	.word vector_table_stub
_prefetch_abort:	.word vector_table_stub
_data_abort:		.word vector_table_stub
_not_used:		.word vector_table_stub
_irq:			.word vector_table_stub
_fiq:			.word vector_table_stub
_pad:			.word 0x12345678 /* now 16*4=64 */
.global _end_vector_table_stub
_end_vector_table_stub:
	.balignl 16,0xdeadbeef
