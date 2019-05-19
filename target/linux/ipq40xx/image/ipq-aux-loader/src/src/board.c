/*
 * Auxiliary kernel loader for Qualcom IPQ-4XXX/806X based boards
 *
 * Copyright (C) 2019 Sergey Sergeev <adron@mstnt.com>
 *
 * Some structures and code has been taken from the U-Boot project.
 *	(C) Copyright 2008 Semihalf
 *	(C) Copyright 2000-2005
 *	Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <printf.h>
#include <iomap.h>
#include <io.h>
#include <types.h>

unsigned long int ntohl(unsigned long int d){
	unsigned long int res = 0;
	int a;
	for(a = 0; a < 3; a++){
		res |= d & 0xFF;
		res <<= 8; d >>= 8;
	}
	res |= d & 0xFF;
	return res;
}

void dump_mem(unsigned char *p, char *str){
	printf("  %s(0x%08X) = %02x %02x %02x %02x %02x %02x %02x %02x\n",
		str,
		p,
		p[0] & 0xFF, p[1] & 0xFF,
		p[2] & 0xFF, p[3] & 0xFF,
		p[4] & 0xFF, p[5] & 0xFF,
		p[6] & 0xFF, p[7] & 0xFF
	);
}

void hang(void)
{
	printf("### ERROR ### Please RESET the board ###\n");
	for (;;);
}

int raise(int signum)
{
	/* Needs for div/mod ops */
	printf("raise: Signal # %d caught\n", signum);
	return 0;
}

/* Replacement (=dummy) for GNU/Linux division-by zero handler */
void __div0 (void)
{
	hang();
}
