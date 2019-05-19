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

#ifndef __ASM_ARM_MINI_IO_H
#define __ASM_ARM_MINI_IO_H

#define dmb()		__asm__ __volatile__ ("" : : : "memory")
#define __iormb()	dmb()
#define __iowmb()	dmb()
#define __arch_putl(v,a)		(*(volatile unsigned int *)(a) = (v))
#define __arch_getl(a)			(*(volatile unsigned int *)(a))

#define writel(v,c)	({ unsigned int __v = v; __iowmb(); __arch_putl(__v,c); __v; })
#define readl(c)	({ unsigned int __v = __arch_getl(c); __iormb(); __v; })

#endif /* __ASM_ARM_MINI_IO_H */
