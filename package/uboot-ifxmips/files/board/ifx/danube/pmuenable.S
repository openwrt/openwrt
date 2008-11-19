/*
 *  Power Management unit initialization code for AMAZON development board.
 *
 *  Copyright (c) 2003	Ou Ke, Infineon.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <version.h>
#include <asm/regdef.h>

#define PMU_PWDCR 		0xBF10201C
#define PMU_SR			0xBF102020

	.globl	pmuenable

pmuenable:
	li      t0, PMU_PWDCR
	li      t1, 0x2		/* enable everything */
	sw      t1, 0(t0)
#if 0
1:
	li	t0, PMU_SR
	lw      t2, 0(t0)
	bne     t1, t2, 1b
	nop
#endif
	j	ra
	nop


