/* Copyright 2004   Manuel Novoa III (mjn3@codepoet.org) */
/* Licensed under the linux kernel's version of the GPL. */

#include <asm/asm.h>
#include <asm/regdef.h>

	.text
	LEAF(startup)
	.set noreorder

	li	t1, BZ_TEXT_START
	add	a0, t1, 0
	la      a1, code_start
	la	a2, code_stop
$L1:
	lw	t0, 0(a1)
	sw	t0, 0(a0)
	add	a1, 4
	add	a0, 4
	blt	a1, a2, $L1

	add	sp, t1, -4
	jal	t1
	
	.set reorder
	END(startup)
