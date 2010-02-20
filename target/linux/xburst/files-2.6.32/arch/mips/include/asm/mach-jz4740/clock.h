/*
 *  linux/include/asm-mips/mach-jz4740/clock.h
 *
 *  JZ4740 clocks definition.
 *
 *  Copyright (C) 2006 - 2007 Ingenic Semiconductor Inc.
 *
 *  Author: <lhhuang@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZ4740_CLOCK_H__
#define __ASM_JZ4740_CLOCK_H__

#include <asm/mach-jz4740/regs.h>

enum jz4740_wait_mode
{
	JZ4740_WAIT_MODE_IDLE,
	JZ4740_WAIT_MODE_SLEEP,
};

void jz4740_clock_set_wait_mode(enum jz4740_wait_mode mode);

void jz4740_clock_udc_enable_auto_suspend(void);
void jz4740_clock_udc_disable_auto_suspend(void);

#endif /* __ASM_JZ4740_CLOCK_H__ */
