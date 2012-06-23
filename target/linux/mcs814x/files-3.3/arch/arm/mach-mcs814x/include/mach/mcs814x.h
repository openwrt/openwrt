/*
 * Copyright (C) 2003 Artec Design Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __ASM_ARCH_MCS814X_H
#define __ASM_ARCH_MCS814X_H

#define MCS814X_IO_BASE		0xF0000000
#define MCS814X_IO_START	0x40000000
#define MCS814X_IO_SIZE		0x00100000

/* IRQ controller register offset */
#define MCS814X_IRQ_ICR		0x00
#define MCS814X_IRQ_ISR		0x04
#define MCS814X_IRQ_MASK	0x20
#define MCS814X_IRQ_STS0	0x40

#define _PHYS_CONFADDR		0x40000000
#define _VIRT_CONFADDR		MCS814X_IO_BASE

#define _CONFOFFSET_UART    0x000DC000
#define _CONFOFFSET_DBGLED  0x000EC000
#define _CONFOFFSET_SYSDBG  0x000F8000

#define _CONFADDR_DBGLED  (_VIRT_CONFADDR + _CONFOFFSET_DBGLED)
#define _CONFADDR_SYSDBG  (_VIRT_CONFADDR + _CONFOFFSET_SYSDBG)

#endif /* __ASM_ARCH_MCS814X_H */

