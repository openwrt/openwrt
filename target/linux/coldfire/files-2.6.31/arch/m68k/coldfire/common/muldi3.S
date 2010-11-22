/*
 * Coldfire muldi3 assembly verion
 * Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *   Jason Jin Jason.Jin@freescale.com
 *   Shrek Wu B16972@freescale.com
 */
 
#include <linux/linkage.h>
.globl __muldi3

ENTRY(__muldi3)
	linkw 	%fp,#0
	lea 	%sp@(-32),%sp
	moveml 	%d2-%d7/%a2-%a3,%sp@
	moveal 	%fp@(8), %a2
	moveal 	%fp@(12), %a3
	moveal  %fp@(16), %a0
	moveal  %fp@(20),%a1
	movel 	%a3,%d2
	andil   #65535,%d2
	movel   %a3,%d3
	clrw 	%d3
	swap 	%d3
	movel 	%a1,%d0
	andil 	#65535,%d0
	movel 	%a1,%d1
	clrw 	%d1
	swap 	%d1
	movel 	%d2,%d7
	mulsl 	%d0,%d7
	movel 	%d2,%d4
	mulsl 	%d1,%d4
	movel 	%d3,%d2
	mulsl 	%d0,%d2
	mulsl 	%d1,%d3
	movel 	%d7,%d0
	clrw 	%d0
	swap 	%d0
	addl 	%d0,%d4
	addl 	%d2,%d4
	cmpl 	%d4,%d2
	blss 	1f
	addil 	#65536,%d3
1:
	movel 	%d4,%d0
	clrw 	%d0
	swap 	%d0
	movel 	%d3,%d5
	addl 	%d0,%d5
	movew 	%d4,%d6
	swap 	%d6
	movew 	%d7,%d6
	movel 	%d5,%d0
	movel 	%d6,%d1
	movel 	%a3,%d2
	movel 	%a0,%d3
	mulsl 	%d3,%d2
	movel 	%a2,%d3
	movel 	%a1,%d4
	mulsl 	%d4,%d3
	addl 	%d3,%d2
	movel 	%d2,%d0
	addl 	%d5,%d0
	moveml 	%sp@, %d2-%d7/%a2-%a3
	lea 	%sp@(32),%sp
	unlk 	%fp
	rts
