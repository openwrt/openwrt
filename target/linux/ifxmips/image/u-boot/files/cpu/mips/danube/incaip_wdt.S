/*
 *  INCA-IP Watchdog timer management code.
 *
 *  Copyright (c) 2003	Wolfgang Denk <wd@denx.de>
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

#ifdef CONFIG_INCA_IP

#define WD_BASE            0xb8000000
#define WD_CON0(value)     0x0020(value)
#define WD_CON1(value)     0x0024(value)
#define WD_DISABLE         0x00000008
#define WD_ENABLE          0x00000000
#define WD_WRITE_PW        0xFFFC00F8
#define WD_WRITE_ENDINIT   0xFFFC00F3
#define WD_WRITE_INIT      0xFFFC00F2


	.globl	disable_incaip_wdt
disable_incaip_wdt:
	li	t0, WD_BASE

	/* Calculate password.
	 */
	lw	t2, WD_CON1(t0)
	and	t2, 0xC

	lw	t3, WD_CON0(t0)
	and	t3, 0xFFFFFF01

	or	t3, t2
	or 	t3, 0xF0

	sw	t3, WD_CON0(t0)		/* write password */

	/* Clear ENDINIT.
	 */
	li	t1, WD_WRITE_INIT
	sw	t1, WD_CON0(t0)


	li	t1, WD_DISABLE
	sw	t1, WD_CON1(t0)		/* disable watchdog */
	li	t1, WD_WRITE_PW
	sw	t1, WD_CON0(t0)		/* write password */
	li	t1, WD_WRITE_ENDINIT
	sw	t1, WD_CON0(t0)		/* end command */

	j	ra
	nop

#endif /* CONFIG_INCA_IP */
