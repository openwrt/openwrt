/*
 *  Realtek RLX based SoC common definitions
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __REALTEK_COMMON_H
#define __REALTEK_COMMON_H

#include <linux/types.h>
#include <linux/io.h>

extern void __iomem *realtek_sys_base;
extern void __iomem *realtek_tc_base;

static inline u32 realtek_sys_read(u32 reg)
{
	return __raw_readl(realtek_sys_base + reg);
}

static inline void realtek_sys_write(u32 reg, u32 val)
{
	__raw_writel(val, realtek_sys_base + reg);
}

static inline u32 realtek_tc_read(u32 reg)
{
	return __raw_readl(realtek_tc_base + reg);
}

void realtek_clocks_init(void);
unsigned long realtek_get_sys_clk_rate(const char *id);

int rtl819x_clockevent_init(void);
int rtl819x_clocksource_init(void);

#endif /* __REALTEK_COMMON_H */
