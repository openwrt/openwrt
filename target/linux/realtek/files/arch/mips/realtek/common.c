/*
 *  Realtek RLX based SoC common routines
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/types.h>

#include <asm/mach-realtek/realtek.h>

enum realtek_soc_type rtl_soc;

void __iomem *realtek_sys_base;
void __iomem *realtek_tc_base;
