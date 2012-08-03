/*
 *   arch/mips/include/asm/mach-lantiq/svip/irq.h
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2010 Lantiq
 *
 */

#ifndef __IRQ_H
#define __IRQ_H

#include <svip_irq.h>

#define NR_IRQS 264

#include_next <irq.h>

/* Functions for EXINT handling */
extern int ifx_enable_external_int(u32 exint, u32 mode);
extern int ifx_disable_external_int(u32 exint);
extern int ifx_external_int_level(u32 exint);

#endif
