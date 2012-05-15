/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/phy.h>
#include <linux/spi/spi_gpio.h>
#include <linux/spi/flash.h>

#include <lantiq_soc.h>
#include <irq.h>

#include "../machtypes.h"
#include "devices.h"
#include "dev-ifxhcd.h"
#include "dev-gpio-leds.h"
#include "dev-gpio-buttons.h"

static struct mtd_partition fritz3370_partitions[] = {
	{
		.name	= "linux",
		.offset	= 0x0,
		.size	= 0x400000,
	},
	{
		.name	= "filesystem",
		.offset	= 0x400000,
		.size	= 0x3000000,
	},
	{
		.name	= "reserved-kernel",
		.offset	= 0x3400000,
		.size	= 0x400000,
	},
	{
		.name	= "reserved",
		.offset	= 0x3800000,
		.size	= 0x3000000,
	},
	{
		.name	= "config",
		.offset	= 0x6800000,
		.size	= 0x200000,
	},
	{
		.name	= "nand-filesystem",
		.offset	= 0x6a00000,
		.size	= 0x1600000,
	},
};

static struct mtd_partition spi_flash_partitions[] = {
	{
		.name	= "urlader",
		.offset	= 0x0,
		.size	= 0x20000,
	},
	{
		.name	= "tffs",
		.offset	= 0x20000,
		.size	= 0x10000,
	},
	{
		.name	= "tffs",
		.offset	= 0x30000,
		.size	= 0x10000,
	},
};

static struct gpio_led
fritz3370_gpio_leds[] __initdata = {
	{ .name = "soc:green:1", .gpio = 32, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:2", .gpio = 33, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:3", .gpio = 34, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:4", .gpio = 35, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:5", .gpio = 36, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:6", .gpio = 47, .active_low = 1, .default_trigger = "default-on" },
};

static struct gpio_keys_button
fritz3370_gpio_keys[] __initdata = {
	{
		.desc = "wifi",
		.type = EV_KEY,
		.code = BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio = 29,
		.active_low = 1,
	},
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode	= PHY_INTERFACE_MODE_RMII,
};

static int usb_pins[2] = { 5, 14 };

#define SPI_GPIO_MRST	16
#define SPI_GPIO_MTSR	17
#define SPI_GPIO_CLK	18
#define SPI_GPIO_CS0	10

static struct spi_gpio_platform_data spi_gpio_data = {
	.sck		= SPI_GPIO_CLK,
	.mosi		= SPI_GPIO_MTSR,
	.miso		= SPI_GPIO_MRST,
	.num_chipselect	= 2,
};

static struct platform_device spi_gpio_device = {
	.name			= "spi_gpio",
	.dev.platform_data	= &spi_gpio_data,
};

static struct flash_platform_data spi_flash_data = {
	.name		= "SPL",
	.parts		= spi_flash_partitions,
	.nr_parts	= ARRAY_SIZE(spi_flash_partitions),
};

static struct spi_board_info spi_flash __initdata = {
	.modalias		= "m25p80",
	.bus_num		= 0,
	.chip_select		= 0,
	.max_speed_hz		= 10 * 1000 * 1000,
	.mode			= SPI_MODE_3,
	.chip_select		= 0,
	.controller_data	= (void *) SPI_GPIO_CS0,
	.platform_data		= &spi_flash_data
};

static void __init
spi_gpio_init(void)
{
	spi_register_board_info(&spi_flash, 1);
	platform_device_register(&spi_gpio_device);
}

static void __init
fritz3370_init(void)
{
	spi_gpio_init();
	platform_device_register_simple("pcie-xway", 0, NULL, 0);
	xway_register_nand(fritz3370_partitions, ARRAY_SIZE(fritz3370_partitions));
	xway_register_hcd(usb_pins);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(fritz3370_gpio_leds), fritz3370_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
		ARRAY_SIZE(fritz3370_gpio_keys), fritz3370_gpio_keys);
	ltq_register_vrx200(&ltq_eth_data);
}

MIPS_MACHINE(LANTIQ_MACH_FRITZ3370,
			"FRITZ3370",
			"FRITZ!BOX 3370",
			fritz3370_init);
