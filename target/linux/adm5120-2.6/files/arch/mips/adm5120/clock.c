/*
 *  $Id$
 *
 *  ADM5120 minimal CLK API implementation
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 *  This file was based on the CLK API implementation in:
 *	arch/mips/tx4938/toshiba_rbtx4938/setup.c
 *	Copyright (C) 2000-2001 Toshiba Corporation
 *	2003-2005 (c) MontaVista Software, Inc.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/clk.h>

#include <asm/mach-adm5120/adm5120_defs.h>

struct clk {
	unsigned long rate;
};

static struct clk uart_clk = {
	.rate = ADM5120_UART_CLOCK
};

struct clk *clk_get(struct device *dev, const char *id)
{
       if (!strcmp(id, "UARTCLK"))
               return &uart_clk;

       return ERR_PTR(-ENOENT);
}
EXPORT_SYMBOL(clk_get);

int clk_enable(struct clk *clk)
{
       return 0;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk)
{
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
       return clk->rate;
}
EXPORT_SYMBOL(clk_get_rate);

void clk_put(struct clk *clk)
{
}
EXPORT_SYMBOL(clk_put);
