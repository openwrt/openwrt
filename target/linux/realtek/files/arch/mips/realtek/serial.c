/*
 *  Realtek RLX based SoC serial devices
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/clk.h>
#include <linux/err.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>
#include "common.h"

static struct resource realtek_uart0_resources[] = {
	{
		.start	= REALTEK_UART0_BASE,
		.end	= REALTEK_UART0_BASE + REALTEK_UART0_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static __maybe_unused struct resource realtek_uart1_resources[] = {
	{
		.start	= REALTEK_UART1_BASE,
		.end	= REALTEK_UART1_BASE + REALTEK_UART1_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

#define REALTEK_UART_FLAGS (UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_IOREMAP)
static struct plat_serial8250_port realtek_uart0_data[] = {
	{
		.mapbase	= REALTEK_UART0_BASE,
		.flags		= REALTEK_UART_FLAGS,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
	}, {
		/* terminating entry */
	}
};

static __maybe_unused struct plat_serial8250_port realtek_uart1_data[] = {
	{
		.mapbase	= REALTEK_UART1_BASE,
		.flags		= REALTEK_UART_FLAGS,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
	}, {
		/* terminating entry */
	}
};

static struct platform_device ath79_uart0_device = {
	.name		= "serial8250",
	.id		= PLAT8250_DEV_PLATFORM,
	.resource	= realtek_uart0_resources,
	.num_resources	= ARRAY_SIZE(realtek_uart0_resources),
	.dev = {
		.platform_data	= realtek_uart0_data
	},
};

static __maybe_unused struct platform_device ath79_uart1_device = {
	.name		= "serial8250",
	.id		= PLAT8250_DEV_PLATFORM,
	.resource	= realtek_uart1_resources,
	.num_resources	= ARRAY_SIZE(realtek_uart1_resources),
	.dev = {
		.platform_data	= realtek_uart1_data
	},
};

void __init realtek_register_uart(void)
{
	unsigned long uart_clk_rate;

	uart_clk_rate = realtek_get_sys_clk_rate("uart");

	if (soc_is_rtl8196c()) {
		realtek_uart0_data[0].irq = REALTEK_SOC_IRQ(7);
		realtek_uart0_data[0].uartclk = uart_clk_rate;
		platform_device_register(&ath79_uart0_device);
	} else {
		BUG();
	}
}
