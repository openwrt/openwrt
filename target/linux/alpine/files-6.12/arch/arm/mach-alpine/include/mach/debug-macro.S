/*
 *  Early serial output macro for Alpine
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

#define DEBUG_ALPINE_SB_BASE		0xfc000000
#define DEBUG_ALPINE_PBS_OFFSET		0x01880000
#define DEBUG_ALPINE_UART_OFFSET	0x3000

		.macro	addruart,rp,rv, tmp
		mov	\rp, #DEBUG_ALPINE_UART_OFFSET
		orr	\rp, \rp, #DEBUG_ALPINE_PBS_OFFSET
		orr	\rv, \rp, #DEBUG_ALPINE_SB_BASE  @ virtual base
		orr	\rp, \rp, #DEBUG_ALPINE_SB_BASE  @ physical base
		.endm

#define UART_SHIFT	2
#include <debug/8250.S>

