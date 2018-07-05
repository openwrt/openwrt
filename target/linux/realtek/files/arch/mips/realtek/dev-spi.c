/*
 *  Realtek SoCs SPI controller device
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include "dev-spi.h"

#include <asm/mach-realtek/platform.h>

static struct resource realtek_spi_resources[] = {
	{
		.start	= REALTEK_SPI_BASE,
		.end	= REALTEK_SPI_BASE + REALTEK_SPI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device realtek_spi_device = {
	.name		= "realtek-spi",
	.id		= -1,
	.resource	= realtek_spi_resources,
	.num_resources	= ARRAY_SIZE(realtek_spi_resources),
};

void __init realtek_register_spi(struct spi_board_info const *info, unsigned n)
{
	spi_register_board_info(info, n);
	platform_device_register(&realtek_spi_device);
}
