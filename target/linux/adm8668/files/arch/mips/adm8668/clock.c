/*
 * ADM8668 minimal clock support
 *
 * Copyright (C) 2012, Florian Fainelli <florian@openwrt.org>
 *
 * Licensed under the terms of the GPLv2
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/clk.h>

#include <adm8668.h>

struct clk {
	unsigned long rate;
};

static struct clk uart_clk = {
	.rate	= ADM8668_UARTCLK_FREQ,
};

struct clk *clk_get(struct device *dev, const char *id)
{
	const char *name = dev_name(dev);

	if (!strcmp(name, "apb:uart0"))
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
