/*
 *  linux/include/asm-mips/mach-jz4740/jz4740.h
 *
 *  JZ4740 common definition.
 *
 *  Copyright (C) 2006 - 2007 Ingenic Semiconductor Inc.
 *
 *  Author: <lhhuang@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZ4740_H__
#define __ASM_JZ4740_H__

#include <asm/mach-jz4740/regs.h>
#include <asm/mach-jz4740/dma.h>

/*------------------------------------------------------------------
 * Platform definitions
 */

#ifdef CONFIG_JZ4740_QI_LB60
#include <asm/mach-jz4740/board-qi_lb60.h>
#endif

/* Add other platform definition here ... */


/*------------------------------------------------------------------
 * Follows are related to platform definitions
 */

#include <asm/mach-jz4740/serial.h>

#endif /* __ASM_JZ4740_H__ */
