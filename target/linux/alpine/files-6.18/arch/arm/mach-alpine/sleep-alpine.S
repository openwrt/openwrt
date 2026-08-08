/*
 * Annapurna labs cpu-idle handler.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/linkage.h>

ENTRY(alpine_suspend_finish)
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #(1 << 2)		@ Disable the C bit
	mcr	p15, 0, r0, c1, c0, 0
	mov r8, lr		@backup lr
	bl v7_flush_dcache_louis
	mov lr, r8
	mrc	p15, 0, r0, c1, c0, 1
	bic	r0, r0, #(1 << 6)		@ Disable SMP bit
	mcr	p15, 0, r0, c1, c0, 1
	isb
	dsb
		@This should shutdown
	wfi
		@We didn't shutdown. probably - we have a pending interrupt.
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #(1 << 2)		@ Enable the C bit
	mcr	p15, 0, r0, c1, c0, 0
	mrc	p15, 0, r0, c1, c0, 1
	orr	r0, r0, #(1 << 6)		@ Enable SMP bit
	mcr	p15, 0, r0, c1, c0, 1
	isb
	dsb
	mov pc, lr
ENDPROC(alpine_suspend_finish)
